/* Copyright (C) 2015, Lancs Inc.
 * Copyright (C) 2009 Trend Micro Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

/* os_xml Library */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "os_xml.h"
#include "os_xml_internal.h"
#include "file_op.h"

/* Prototypes */
static int _oscomment(OS_XML *_lxml) __attribute__((nonnull));
static int _writecontent(const char *str, size_t size, unsigned int parent, OS_XML *_lxml) __attribute__((nonnull));
static int _writememory(const char *str, XML_TYPE type, size_t size,
                        unsigned int parent, OS_XML *_lxml) __attribute__((nonnull));
static int _xml_fgetc(FILE *fp, OS_XML *_lxml) __attribute__((nonnull));
int _xml_sgetc(OS_XML *_lxml)  __attribute__((nonnull));
static int _getattributes(unsigned int parent, OS_XML *_lxml) __attribute__((nonnull));
static void xml_error(OS_XML *_lxml, const char *msg, ...) __attribute__((format(printf, 2, 3), nonnull));

/**
 * @brief Recursive method to read XML elements.
 *
 * @param parent Current depth.
 * @param _lxml XML structure.
 * @param recursion_level Max recursion level allowed.
 * @param flag_truncate If TRUE, truncates the content of a tag when it's bigger than XML_MAXSIZE. Fails if set to FALSE.
 * @return int Returns 0, -1 or -2.
 */
static int _ReadElem(unsigned int parent, OS_XML *_lxml, unsigned int recursion_level, bool flag_truncate) __attribute__((nonnull));

/* Local fgetc */
static int _xml_fgetc(FILE *fp, OS_XML *_lxml)
{
    int c;

    // If there is any character in the stash, get it
    c = (_lxml->stash_i > 0) ? _lxml->stash[--_lxml->stash_i] : fgetc(fp);

    if (c == '\n') { /* add newline */
        _lxml->line++;
    }

    return (c);
}

int _xml_sgetc(OS_XML *_lxml){
    int c;

    if (_lxml->stash_i > 0) {
        c = _lxml->stash[--_lxml->stash_i];
    }
    else if (_lxml->string) {
        c = *(_lxml->string++);
    }
    else {
        c = -1;
    }

    if (c == '\n') { /* add newline */
        _lxml->line++;
    }

    return c;
}

static int _xml_ungetc(int c, OS_XML *_lxml)
{
    // If stash is full, give up
    if (_lxml->stash_i >= XML_STASH_LEN) {
        return -1;
    }

    _lxml->stash[_lxml->stash_i++] = c;

    if (c == '\n') { /* substract newline */
        _lxml->line--;
    }

    return 0;
}

static void xml_error(OS_XML *_lxml, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    memset(_lxml->err, '\0', XML_ERR_LENGTH);
    vsnprintf(_lxml->err, XML_ERR_LENGTH - 1, msg, args);
    va_end(args);
    _lxml->err_line = _lxml->line;
}

/* Clear memory */
void OS_ClearXML(OS_XML *_lxml)
{
    unsigned int i;
    for (i = 0; i < _lxml->cur; i++) {
        free(_lxml->el[i]);
        free(_lxml->ct[i]);
    }
    _lxml->cur = 0;
    _lxml->fol = 0;
    _lxml->err_line = 0;

    free(_lxml->el);
    _lxml->el = NULL;

    free(_lxml->ct);
    _lxml->ct = NULL;

    free(_lxml->rl);
    _lxml->rl = NULL;

    free(_lxml->tp);
    _lxml->tp = NULL;

    free(_lxml->ck);
    _lxml->ck = NULL;

    free(_lxml->ln);
    _lxml->ln = NULL;

    memset(_lxml->err, '\0', XML_ERR_LENGTH);
    _lxml->line = 0;
    _lxml->stash_i = 0;
}

int ParseXML(OS_XML *_lxml, bool flag_truncate) {
    int r;
    unsigned int i;
    char *str_base = _lxml->string;

    /* Zero the line */
    _lxml->line = 1;

    // Reset stash
    _lxml->stash_i = 0;

    if ((r = _ReadElem(0, _lxml, 0, flag_truncate)) < 0) { /* First position */
        if (r != LEOF) {

            if(_lxml->fp){
                fclose(_lxml->fp);
            } else if (str_base){
                free(str_base);
            }

            return (-1);
        }
    }

    for (i = 0; i < _lxml->cur; i++) {
        if (_lxml->ck[i] == 0) {
            xml_error(_lxml, "XMLERR: Element '%s' not closed.", _lxml->el[i]);

            if(_lxml->fp){
                fclose(_lxml->fp);
            } else if (str_base){
                free(str_base);
            }

            return (-1);
        }
    }

    if(_lxml->fp){
        fclose(_lxml->fp);
    } else if (str_base){
        free(str_base);
    }

    return (0);
}

int OS_ReadXMLString_Ex(const char *string, OS_XML *_lxml, bool flag_truncate){
    /* Initialize xml structure */
    memset(_lxml, 0, sizeof(OS_XML));

    _lxml->string = strdup(string);
    _lxml->fp = NULL;

    return ParseXML(_lxml, flag_truncate);
}

int OS_ReadXMLString(const char *string, OS_XML *_lxml){
    return OS_ReadXMLString_Ex(string, _lxml, false);
}

/* Read a XML file and generate the necessary structs */
int OS_ReadXML_Ex(const char *file, OS_XML *_lxml, bool flag_truncate) {
    FILE *fp;

    /* Initialize xml structure */
    memset(_lxml, 0, sizeof(OS_XML));

    fp = fopen(file, "r");
    if (!fp) {
        xml_error(_lxml, "XMLERR: File '%s' not found.", file);
        return (-2);
    }
   // fclose(fp); //should be change to w_file_cloexec()
    _lxml->fp = fp;
    _lxml->string = NULL;

    return ParseXML(_lxml, flag_truncate);
}

int OS_ReadXML(const char *file, OS_XML *_lxml) {
    return OS_ReadXML_Ex(file, _lxml, false);
}

static int _oscomment(OS_XML *_lxml)
{
    int c;
    if ((c = xml_getc_fun(_lxml->fp, _lxml)) == _R_COM) {
        while ((c = xml_getc_fun(_lxml->fp, _lxml)) != EOF) {
            if (c == _R_COM) {
                if ((c = xml_getc_fun(_lxml->fp, _lxml)) == _R_CONFE) {
                    return (1);
                }
                _xml_ungetc(c, _lxml);
            } else if (c == '-') {  /* W3C way of finishing comments */
                if ((c = xml_getc_fun(_lxml->fp, _lxml)) == '-') {
                    if ((c = xml_getc_fun(_lxml->fp, _lxml)) == _R_CONFE) {
                        return (1);
                    }
                    _xml_ungetc(c, _lxml);
                }
                _xml_ungetc(c, _lxml);
            } else {
                continue;
            }
        }
        return (-1);
    } else {
        _xml_ungetc(c, _lxml);
    }
    return (0);
}

static int _ReadElem(unsigned int parent, OS_XML *_lxml, unsigned int recursion_level, bool flag_truncate) {
    int c=0;
    unsigned int count = 0;
    unsigned int _currentlycont = 0;
    short int location = -1;
    int cmp = 0;
    int retval = -1;
    bool ignore_content = false;

    int prevv = 1;
    char *elem = NULL;
    char *cont = NULL;
    char *closedelim = NULL;
    if (++recursion_level > 1024) {
        // 1024 levels should be enough for configuration and eventchannel events
        xml_error(_lxml, "XMLERR: Max recursion level reached");
        return -1;
    }

    elem = calloc(XML_MAXSIZE + 1, sizeof(char));
    cont = calloc(XML_MAXSIZE + 1, sizeof(char));
    closedelim = calloc(XML_MAXSIZE + 1, sizeof(char));

    if (elem == NULL || cont == NULL || closedelim == NULL) {
        goto end;
    }

    if (_lxml->fp){
        cmp = EOF;
        _lxml->string = NULL;
    } else if (_lxml->string){
        cmp = '\0';
    }

    while ((c = xml_getc_fun(_lxml->fp, _lxml)) != cmp) {
        if (c == '\\') {
            prevv *= -1;
        } else if (c != _R_CONFS && prevv == -1){
            prevv = 1;
        }

        /* Max size */
        if (count >= XML_MAXSIZE) {
            if (flag_truncate && 1 == location) {
                ignore_content = true;
            } else {
                xml_error(_lxml, "XMLERR: String overflow.");
                goto end;
            }
        }

        /* Check for comments */
        if (c == _R_CONFS) {
            int r = 0;
            if ((r = _oscomment(_lxml)) < 0) {
                xml_error(_lxml, "XMLERR: Comment not closed.");
                goto end;
            } else if (r == 1) {
                continue;
            }
        }

        /* Real checking */
        if ((location == -1) && (prevv == 1)) {
            if (c == _R_CONFS) {
                if ((c = xml_getc_fun(_lxml->fp, _lxml)) == '/') {
                    xml_error(_lxml, "XMLERR: Element not opened.");
                    goto end;
                } else {
                    _xml_ungetc(c, _lxml);
                }
                location = 0;
            } else {
                continue;
            }
        }

        else if ((location == 0) && ((c == _R_CONFE) || isspace(c))) {
            int _ge = 0;
            int _ga = 0;
            elem[count] = '\0';

            /* Remove the / at the end of the element name */
            if (count > 0 && elem[count - 1] == '/') {
                _ge = '/';
                elem[count - 1] = '\0';
            }

            if (_writememory(elem, XML_ELEM, count + 1, parent, _lxml) < 0) {
                goto end;
            }
            _currentlycont = _lxml->cur - 1;
            if (isspace(c)) {
                if ((_ga = _getattributes(parent, _lxml)) < 0) {
                    goto end;
                }
            }

            /* If the element is closed already (finished in />) */
            if ((_ge == '/') || (_ga == '/')) {
                if (_writecontent("\0", 2, _currentlycont, _lxml) < 0) {
                    goto end;
                }
                _lxml->ck[_currentlycont] = 1;
                _currentlycont = 0;
                count = 0;
                location = -1;

                memset(elem, '\0', XML_MAXSIZE);
                memset(closedelim, '\0', XML_MAXSIZE);
                memset(cont, '\0', XML_MAXSIZE);

                if (parent > 0) {
                    retval = 0;
                    goto end;
                }
            } else {
                count = 0;
                location = 1;
            }
        }

        else if ((location == 2) && (c == _R_CONFE)) {
            closedelim[count] = '\0';
            if (strcmp(closedelim, elem) != 0) {
                xml_error(_lxml, "XMLERR: Element '%s' not closed.", elem);
                goto end;
            }
            if (_writecontent(cont, strlen(cont) + 1, _currentlycont, _lxml) < 0) {
                goto end;
            }
            _lxml->ck[_currentlycont] = 1;
            memset(elem, '\0', XML_MAXSIZE);
            memset(closedelim, '\0', XML_MAXSIZE);
            memset(cont, '\0', XML_MAXSIZE);
            _currentlycont = 0;
            count = 0;
            location = -1;
            if (parent > 0) {
                retval = 0;
                goto end;
            }
        } else if ((location == 1) && (c == _R_CONFS) && (prevv == 1)) {
            if ((c = xml_getc_fun(_lxml->fp, _lxml)) == '/') {
                cont[count] = '\0';
                count = 0;
                location = 2;
                ignore_content = false;
            } else {
                _xml_ungetc(c, _lxml);
                _xml_ungetc(_R_CONFS, _lxml);

                if (_ReadElem(parent + 1, _lxml, recursion_level, flag_truncate) < 0) {
                    goto end;
                }
                count = 0;
            }
        } else {
            if (location == 0) {
                elem[count++] = (char) c;
            } else if (location == 1 && !ignore_content) {
                cont[count++] = (char) c;
            } else if (location == 2) {
                closedelim[count++] = (char) c;
            }

            if (_R_CONFS == c) {
                prevv = 1;
            }
        }
    }
    if (location == -1) {
        retval = LEOF;
    }

    xml_error(_lxml, "XMLERR: End of file and some elements were not closed.");

end:
    if (elem) {
        free(elem);
    }

    if (cont) {
        free(cont);
    }

    if (closedelim) {
        free(closedelim);
    }

    return retval;
}

static int _writememory(const char *str, XML_TYPE type, size_t size,
                        unsigned int parent, OS_XML *_lxml)
{
    char **tmp;
    int *tmp2;
    unsigned int *tmp3;
    XML_TYPE *tmp4;

    /* Allocate for the element */
    tmp = (char **)realloc(_lxml->el, (_lxml->cur + 1) * sizeof(char *));
    if (tmp == NULL) {
        goto fail;
    }
    _lxml->el = tmp;
    _lxml->el[_lxml->cur] = (char *)calloc(size, sizeof(char));
    if (_lxml->el[_lxml->cur] == NULL) {
        goto fail;
    }
    strncpy(_lxml->el[_lxml->cur], str, size - 1);

    /* Allocate for the content */
    tmp = (char **)realloc(_lxml->ct, (_lxml->cur + 1) * sizeof(char *));
    if (tmp == NULL) {
        goto fail;
    }
    _lxml->ct = tmp;
    _lxml->ct[_lxml->cur] = NULL;

    /* Allocate for the type */
    tmp4 = (XML_TYPE *) realloc(_lxml->tp, (_lxml->cur + 1) * sizeof(XML_TYPE));
    if (tmp4 == NULL) {
        goto fail;
    }
    _lxml->tp = tmp4;
    _lxml->tp[_lxml->cur] = type;

    /* Allocate for the relation */
    tmp3 = (unsigned int *) realloc(_lxml->rl, (_lxml->cur + 1) * sizeof(unsigned int));
    if (tmp3 == NULL) {
        goto fail;
    }
    _lxml->rl = tmp3;
    _lxml->rl[_lxml->cur] = parent;

    /* Allocate for the "check" */
    tmp2 = (int *) realloc(_lxml->ck, (_lxml->cur + 1) * sizeof(int));
    if (tmp2 == NULL) {
        goto fail;
    }
    _lxml->ck = tmp2;
    _lxml->ck[_lxml->cur] = 0;

    /* Allocate for the line */
    tmp3 = (unsigned int *) realloc(_lxml->ln, (_lxml->cur + 1) * sizeof(unsigned int));
    if (tmp3 == NULL) {
        goto fail;
    }
    _lxml->ln = tmp3;
    _lxml->ln[_lxml->cur] = _lxml->line;

    /* Attributes does not need to be closed */
    if (type == XML_ATTR) {
        _lxml->ck[_lxml->cur] = 1;
    }

    /* Check if it is a variable */
    if (strcasecmp(XML_VAR, str) == 0) {
        _lxml->tp[_lxml->cur] = XML_VARIABLE_BEGIN;
    }

    _lxml->cur++;
    return (0);

fail:
    snprintf(_lxml->err, XML_ERR_LENGTH, "XMLERR: Memory error.");
    return (-1);
}

static int _writecontent(const char *str, size_t size, unsigned int parent, OS_XML *_lxml)
{
    _lxml->ct[parent] = (char *)calloc(size, sizeof(char));
    if ( _lxml->ct[parent] == NULL) {
        snprintf(_lxml->err, XML_ERR_LENGTH, "XMLERR: Memory error.");
        return (-1);
    }
    strncpy(_lxml->ct[parent], str, size - 1);

    return (0);
}

/* Read the attributes of an element */
static int _getattributes(unsigned int parent, OS_XML *_lxml)
{
    int location = 0;
    unsigned int count = 0;
    int c;
    int c_to_match = 0;

    char attr[XML_MAXSIZE + 1];
    char value[XML_MAXSIZE + 1];

    memset(attr, '\0', XML_MAXSIZE + 1);
    memset(value, '\0', XML_MAXSIZE + 1);

    while ((c = xml_getc_fun(_lxml->fp, _lxml)) != EOF) {
        if (count >= XML_MAXSIZE) {
            attr[count - 1] = '\0';
            xml_error(_lxml,
                      "XMLERR: Overflow attempt at attribute '%.20s'.", attr);
            return (-1);
        }

        else if ((c == _R_CONFE) || ((location == 0) && (c == '/'))) {
            if (location == 1) {
                xml_error(_lxml, "XMLERR: Attribute '%s' not closed.",
                          attr);
                return (-1);
            } else if ((location == 0) && (count > 0)) {
                xml_error(_lxml, "XMLERR: Attribute '%s' has no value.",
                          attr);
                return (-1);
            } else if (c == '/') {
                return (c);
            } else {
                return (0);
            }
        } else if ((location == 0) && (c == '=')) {
            attr[count] = '\0';

            /* Check for existing attribute with same name */
            unsigned int i = _lxml->cur - 1;
            /* Search attributes backwards in same parent */
            while (_lxml->rl[i] == parent && _lxml->tp[i] == XML_ATTR) {
                if (strcmp(_lxml->el[i], attr) == 0) {
                    xml_error(_lxml, "XMLERR: Attribute '%s' already defined.", attr);
                    return (-1);
                }

                /* Continue with previous element */
                if (i == 0) {
                    break;
                }
                i--;
            }

            c = xml_getc_fun(_lxml->fp, _lxml);
            if ((c != '"') && (c != '\'')) {
                unsigned short int _err = 1;
                if (isspace(c)) {
                    while ((c = xml_getc_fun(_lxml->fp, _lxml)) != EOF) {
                        if (isspace(c)) {
                            continue;
                        } else if ((c == '"') || (c == '\'')) {
                            _err = 0;
                            break;
                        } else {
                            break;
                        }
                    }
                }
                if (_err != 0) {
                    xml_error(_lxml,
                              "XMLERR: Attribute '%s' not followed by a \" or \'."
                              , attr);
                    return (-1);
                }
            }

            c_to_match = c;
            location = 1;
            count = 0;
        } else if ((location == 0) && (isspace(c))) {
            if (count == 0) {
                continue;
            } else {
                xml_error(_lxml, "XMLERR: Attribute '%s' has no value.", attr);
                return (-1);
            }
        } else if ((location == 1) && (c == c_to_match)) {
            value[count] = '\0';
            if (_writememory(attr, XML_ATTR, strlen(attr) + 1,
                             parent, _lxml) < 0) {
                return (-1);
            }
            if (_writecontent(value, count + 1, _lxml->cur - 1, _lxml) < 0) {
                return (-1);
            }
            c = xml_getc_fun(_lxml->fp, _lxml);
            if (isspace(c)) {
                return (_getattributes(parent, _lxml));
            } else if (c == _R_CONFE) {
                return (0);
            } else if (c == '/') {
                return (c);
            }

            xml_error(_lxml,
                      "XMLERR: Bad attribute closing for '%s'='%s'.",
                      attr, value);
            return (-1);
        } else if (location == 0) {
            attr[count++] = (char) c;
        } else if (location == 1) {
            value[count++] = (char) c;
        }
    }

    xml_error(_lxml, "XMLERR: End of file while reading an attribute.");
    return (-1);
}

const char * w_get_attr_val_by_name(xml_node * node, const char * name) {

    if (!node || !node->attributes || !name) {
        return NULL;
    }

    for (int i = 0; node->attributes[i]; i++) {
        if (strcmp(node->attributes[i], name) == 0) {
            return node->values[i];
        }
    }

    return NULL;
}


int ReadConfig(int modules,const char *cfgfile, void *d1,void *d2)
{
    int i;
    OS_XML xml;
    XML_NODE node;
    /*define root node of xml*/
    const char *xml_root = "dlp_config";
    /*Read content of config file and store in xml structure*/
    if(OS_ReadXML(cfgfile,&xml) < 0)
    {
        /*replace later*/
        printf("Error read xml file\n");
        return(-1);
    }
    /*Get root node element */
    node = OS_GetElementsbyNode(&xml,NULL);
    if(!node)
    {
        return 0;
    }
    /*If root node doesn't have element*/
    if(!node[0]->element)
    {
        /*print error, clear node and exit*/
        printf("Invalid NULL element\n");
        OS_ClearNode(node);
        OS_ClearXML(&xml);
        return(-1);
    }
    else{
        if(strcmp(node[0]->element,xml_root) == 0)
        {
            XML_NODE child_node = NULL;
            child_node = OS_GetElementsbyNode(&xml,node[0]);
            if(child_node)
            {
                if(read_main_elements(&xml,modules,child_node,d1,d2) < 0)
                {
                    printf("Configuration error \n");
                    OS_ClearNode(child_node);
                    OS_ClearNode(node);
                    OS_ClearXML(&xml);
                    return(-1);
                }
                OS_ClearNode(child_node);
            }
        }
        else{
            printf("Invalid element %s\n",node[0]->element);
            OS_ClearNode(node);
            OS_ClearXML(&xml);
            return (-1);
        }
    }
    /*if read successfully*/
    OS_ClearNode(node);
    OS_ClearXML(&xml);
    return (0);
}
/*
If xml_path doesn't exist this function will create new xml_path and insert newval to it
NOTE: Only use this function to change value of xml node which has UNIQUE xml_path
*/
int change_config_value(const char* cfgfile,const char **xml_path,char *newval)
{
    if(!xml_path)
    {
        printf("Error: empty xml_path\n");
        return -1;
    }
    int count = 0;
    while (xml_path[count] != NULL) {
        count++;
    }
    printf("working on xml_node %s\n",xml_path[count-1]);
    char *config_tmp = cfgfile;
    char *conf_file = basename(config_tmp);
    char tmp_path[strlen(TMP_DIR) + 1 + strlen(conf_file) + 6 + 1];
    snprintf(tmp_path, sizeof(tmp_path), "%s/%sXXXXXX", TMP_DIR, conf_file);
    if (mkstemp(tmp_path) == -1)
    {
        printf( "Could not create temporary file.");
        return (0);
    }
    /* Read the XML. Print error and line number. */
    if (OS_WriteXML(cfgfile, tmp_path, xml_path, NULL, newval) != 0)
    {
        printf("Unable to set %s.\r\n",xml_path[count-1]);
        if (unlink(tmp_path))
        {
           printf("Could not delete temporary file.");
        }

        return (0);
    }
     /* Rename config files */
    if (rename(cfgfile, LASTCONFIG))
    {
        printf("Unable to backup configuration.");
        if (unlink(tmp_path))
        {
            printf("Could not delete temporary file.");
        }

        return (0);
    }

    if (rename(tmp_path, cfgfile))
    {
        
        if (unlink(tmp_path))
        {
           printf("cannot delete temp file\n");
        }

        return (0);
    }

    return (1);
}
/*Trim off string */
char *trimString(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}
int count_spaces(const char *line)
{
    int count = 0;
    while (*line && *line == ' ') {
        count++;
        line++;
    }
    return count;
}
char *prewrite_to_file(char buf[],int numspaces,const char *topic)
{
    char temp[BUF_MAX];
    /*begin prefix of temp buffer to buf*/
    snprintf(temp,BUF_MAX,STRINGINSR,topic);
    /*fill up buf with spaces first*/
    for(int i=0;i<numspaces;i++)
    {
        buf[i] = ' ';
    }
    /*copy temp buffer content to buffer*/
    for(int i=0,j=numspaces;i<BUF_MAX,j<BUF_MAX-numspaces;i++,j++)
    {
        buf[j] = temp[i]; 
    }
    return buf;

}

int Write_topic_to_conf(const char *infile, const char *outfile, char *topic,int module)
{
    FILE *fp_in;
    FILE *fp_out;
    fp_in = fopen(infile, "r");
    if (!fp_in)
    {
        return (XMLW_NOIN);
    }

    setvbuf(fp_in, NULL, _IOFBF, MAXSIZE + 1);

    /* Open outfile */
    fp_out = fopen(outfile, "w");
    if (!fp_out)
    {
        fclose(fp_in);
        return (XMLW_NOOUT);
    }
    char *matchstr = NULL;
    if(module == PUB){
       matchstr = strdup(MATCHPUB);
    }
    if(module == SUB){
        matchstr = strdup(MATCHSUB);
    }
    bool keep_reading = true;
    char buffer[BUF_MAX];
    char temp_buf[BUF_MAX];
    bool check = false;
    do
    {
        fgets(buffer, BUF_MAX, fp_in); /*Read file config line by line*/
        memcpy(temp_buf, buffer, sizeof(buffer));
        char *temp_buff = trimString(temp_buf);/*Trim off buffer to compare*/
        if (feof(fp_in)){/*end of file then stop reading*/
            keep_reading = false;
        }
        if( strcmp(temp_buff, matchstr) == 0){
            int numspaces = 0;
            char insertline[BUF_MAX];// stored line to print to config file
            memset(insertline,0,sizeof(insertline));
            numspaces = count_spaces(buffer);/*count number of spaces to print to conf*/
            prewrite_to_file(insertline,numspaces + 4,topic);
            fputs(buffer,fp_out);
            fprintf(fp_out, "%s",insertline);
        }
        else
        {
            fputs(buffer, fp_out);
        }
    } while (keep_reading);
    fclose(fp_in);
    fclose(fp_out);

    return (0);
}

int add_new_topic(const char *cfgfile,char *topic,int module)
{
    char *config_tmp = cfgfile;
    char *conf_file = basename(config_tmp);
    char tmp_path[strlen(TMP_DIR) + 1 + strlen(conf_file) + 6 + 1];
    snprintf(tmp_path, sizeof(tmp_path), "%s/%sXXXXXX", TMP_DIR, conf_file);
    if (mkstemp(tmp_path) == -1)
    {
        printf( "Could not create temporary file.");
        return (0);
    }
    /* Read the XML. Print error and line number. */
    if (Write_topic_to_conf(cfgfile, tmp_path,topic,module) != 0)
    {
        printf("Unable to set topic %s.\r\n",topic);
        if (unlink(tmp_path))
        {
           printf("Could not delete temporary file.");
        }

        return (0);
    }
     /* Rename config files */
    if (rename(cfgfile, LASTCONFIG))
    {
        printf("Unable to backup configuration.");
        if (unlink(tmp_path))
        {
            printf("Could not delete temporary file.");
        }

        return (0);
    }

    if (rename(tmp_path, cfgfile))
    {
        
        if (unlink(tmp_path))
        {
           printf("cannot delete temp file\n");
        }

        return (0);
    }

    return (1);
}

int change_config_value_rep(const char* cfgfile,const char **xml_path,char *oldval,char*newval)
{
    if(!xml_path)
    {
        printf("Error: empty xml_path\n");
        return -1;
    }
    int count = 0;
    while (xml_path[count] != NULL) {
        count++;
    }
    printf("working on xml_node %s\n",xml_path[count-1]);
    char *config_tmp = cfgfile;
    char *conf_file = basename(config_tmp);
    char tmp_path[strlen(TMP_DIR) + 1 + strlen(conf_file) + 6 + 1];
    snprintf(tmp_path, sizeof(tmp_path), "%s/%sXXXXXX", TMP_DIR, conf_file);
    if (mkstemp(tmp_path) == -1)
    {
        printf( "Could not create temporary file.");
        return (0);
    }
    /* Read the XML. Print error and line number. */
    if (OS_WriteXML(cfgfile, tmp_path, xml_path, oldval, newval) != 0)
    {
        printf("Unable to set %s.\r\n",xml_path[count-1]);
        if (unlink(tmp_path))
        {
           printf("Could not delete temporary file.");
        }

        return (0);
    }
     /* Rename config files */
    if (rename(cfgfile, LASTCONFIG))
    {
        printf("Unable to backup configuration.");
        if (unlink(tmp_path))
        {
            printf("Could not delete temporary file.");
        }

        return (0);
    }

    if (rename(tmp_path, cfgfile))
    {
        
        if (unlink(tmp_path))
        {
           printf("cannot delete temp file\n");
        }

        return (0);
    }

    return (1);
}
char *get_topic_content(char xml[])
{
    unsigned int i = 0;
    char * buffer = NULL;
    buffer = (char*)malloc(BUF_MAX);
    if(!buffer)
    {
        printf("Cannot allocate buffer\n");
    }
    while(xml[i] != '>')/*find index of '>'*/
    {
        i++;
    }
    //int first = i + 1; /*save first pos of wanted cut string*/
    unsigned int j = 0;
     while(xml[i+1] != '<'){
        buffer[j] = xml[i+1];
        i++;
        j++;
    }
    buffer[j] = '\0';
    return buffer;
}


bool check_tag_is_topic(char xml[])
{
    bool check = false;
    unsigned int i=0;
    char buffer[BUF_MAX];
    while(xml[i] != '<')/*find index of open tag '<' */
    {
        i++;
    } 
    unsigned int j = 0;
    while(xml[j] != '>'){/*find index of close tag '>' */
        j++;
    }
    memcpy(buffer,xml+i,j-i+1);
    buffer[j-i+1] = '\0';
    if(strcmp(buffer,TOPIC) == 0)
    {
        check = true;
    }
    return check;
}

int Delete_topic(const char *infile, const char *outfile, const char *topic)
{
    FILE *fp_in;
    FILE *fp_out;

    /* Open infile */
    fp_in = fopen(infile, "r");
    if (!fp_in)
    {
        printf("cannot open test.conf\n");
        exit(-1);
    }

    setvbuf(fp_in, NULL, _IOFBF, MAXSIZE + 1);

    /* Open outfile */
    fp_out = fopen(outfile, "w");
    if (!fp_out)
    {
        fclose(fp_in);
        printf("cannot open change.conf\n");
        exit(-1);
    }
    bool keep_reading = true;
    do
    {
        char buffer[BUF_MAX];
        memset(buffer,0,BUF_MAX);
        fgets(buffer, BUF_MAX, fp_in);
        if (feof(fp_in)){
            keep_reading = false;
        }
        if(check_tag_is_topic(buffer)){/*find tag <topic> to process*/
            char* temp = NULL;
            temp = get_topic_content(buffer);/*get content of tag */
            char *res = trimString(temp);
            if (strcmp(res, topic) == 0)/*found matching topic to remove*/
            {
               free(temp);
               continue;
            }else{ 
                fputs(buffer, fp_out);
                free(temp);
            }
        }
        else{
            fputs(buffer, fp_out);
        }
    } while (keep_reading);
    fclose(fp_in);
    fclose(fp_out);
    return (0);
}

int delete_topic_from_conf(const char *cfgfile,char *topic)
{
    char *config_tmp = cfgfile;
    char *conf_file = basename(config_tmp);
    char tmp_path[strlen(TMP_DIR) + 1 + strlen(conf_file) + 6 + 1];
    snprintf(tmp_path, sizeof(tmp_path), "%s/%sXXXXXX", TMP_DIR, conf_file);
    if (mkstemp(tmp_path) == -1)
    {
        printf( "Could not create temporary file.");
        return (0);
    }
    /* Read the XML. Print error and line number. */
    if (Delete_topic(cfgfile, tmp_path,topic) != 0)
    {
        printf("Unable to set topic %s.\r\n",topic);
        if (unlink(tmp_path))
        {
           printf("Could not delete temporary file.");
        }

        return (0);
    }
     /* Rename config files */
    if (rename(cfgfile, LASTCONFIG))
    {
        printf("Unable to backup configuration.");
        if (unlink(tmp_path))
        {
            printf("Could not delete temporary file.");
        }

        return (0);
    }

    if (rename(tmp_path, cfgfile))
    {
        
        if (unlink(tmp_path))
        {
           printf("cannot delete temp file\n");
        }

        return (0);
    }

    return (1);
}