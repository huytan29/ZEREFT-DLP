/* Copyright (C) 2015, Lancs Inc.
 * Copyright (C) 2009 Trend Micro Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

/* os_xml C Library */

#ifndef OS_XML_H
#define OS_XML_H

#include <stdio.h>
#include <stdbool.h>
#include <mosquitto.h>
#include <sys/time.h>


/*Define modules we use in DLP*/
#define PUB 00000001
#define SUB 00000002
/*Declare mosquitto config struct*/
#define MAXSIZE 10240
#define BUF_MAX 1024
#define MATCHPUB "<pub>\0"
#define MATCHSUB "<sub>\0"
#define TOPIC "<topic>"
#define STRINGINSR "<topic>%s</topic>\n"
#define TMP_DIR "/home/lancs/DataLossPrevention/codebase/Rwconfig"
#define LASTCONFIG "last-lancs_dlp.conf"


/* XML Node structure */
typedef struct _xml_node {
    unsigned int key;
    char *element;
    char *content;
    char **attributes;
    char **values;
} xml_node;

#define XML_ERR_LENGTH  128
#define XML_STASH_LEN   2
#define xml_getc_fun(x,y) (x)? _xml_fgetc(x,y) : _xml_sgetc(y)
typedef enum _XML_TYPE { XML_ATTR, XML_ELEM, XML_VARIABLE_BEGIN = '$' } XML_TYPE;

/* XML structure */
typedef struct _OS_XML {
    unsigned int cur;           /* Current position (and last after reading) */
    int fol;                    /* Current position for the xml_access */
    XML_TYPE *tp;               /* Item type */
    unsigned int *rl;           /* Relation in the XML */
    int *ck;                    /* If the item was closed or not */
    unsigned int *ln;           /* Current xml file line */
    unsigned int err_line;      /* Line number of the possible error */
    char **ct;                  /* Content is stored */
    char **el;                  /* The element/attribute name is stored */
    char err[XML_ERR_LENGTH];   /* Error messages are stored in here */
    unsigned int line;          /* Current line */
    char stash[XML_STASH_LEN];  /* Ungot characters stash */
    int stash_i;                /* Stash index */
    FILE *fp;                   /* File descriptor */
    char *string;               /* XML string */
} OS_XML;

typedef xml_node **XML_NODE;

/**
 * @brief Parses a XML file and stores the content in the OS_XML struct.
 *        This legacy method will always fail if the content of a tag is bigger than XML_MAXSIZE.
 *
 * @param file The source file to read.
 * @param lxml The struct to store the result.
 * @return int OS_SUCCESS on success, OS_INVALID otherwise.
 */
int OS_ReadXML(const char *file, OS_XML *lxml) __attribute__((nonnull));

/**
 * @brief Parses a XML file and stores the content in the OS_XML struct.
 *
 * @param file The source file to read.
 * @param lxml The struct to store the result.
 * @param flag_truncate Toggles the behavior in case of the content of a tag is bigger than XML_MAXSIZE:
 *                      truncate on TRUE or fail on FALSE.
 * @return int OS_SUCCESS on success, OS_INVALID otherwise.
 */
int OS_ReadXML_Ex(const char *file, OS_XML *_lxml, bool flag_truncate) __attribute__((nonnull));

/**
 * @brief Parses a XML string and stores the content in the OS_XML struct.
 *        This legacy method will always fail if the content of a tag is bigger than XML_MAXSIZE.
 *
 * @param string The source string to read.
 * @param lxml The struct to store the result.
 * @return int OS_SUCCESS on success, OS_INVALID otherwise.
 */
int OS_ReadXMLString(const char *string, OS_XML *_lxml) __attribute__((nonnull));

/**
 * @brief Parses a XML string and stores the content in the OS_XML struct.
 *
 * @param string The source string to read.
 * @param lxml The struct to store the result.
 * @param flag_truncate Toggles the behavior in case of the content of a tag is bigger than XML_MAXSIZE:
 *                      truncate on TRUE or fail on FALSE.
 * @return int OS_SUCCESS on success, OS_INVALID otherwise.
 */
int OS_ReadXMLString_Ex(const char *string, OS_XML *_lxml, bool flag_truncate) __attribute__((nonnull));

/**
 * @brief Parses a XML string or file.
 *
 * @param _lxml The xml struct.
 * @param flag_truncate Toggles the behavior in case of the content of a tag is bigger than XML_MAXSIZE:
 *                      truncate on TRUE or fail on FALSE.
 * @return int OS_SUCCESS on success, OS_INVALID otherwise.
 */
int ParseXML(OS_XML *_lxml, bool flag_truncate) __attribute__((nonnull));

/* Clear the XML structure memory */
void OS_ClearXML(OS_XML *_lxml) __attribute__((nonnull));

/* Clear a node */
void OS_ClearNode(xml_node **node);


/* Functions to read the XML */

/* Return 1 if element_name is a root element */
unsigned int OS_RootElementExist(const OS_XML *_lxml, const char *element_name) __attribute__((nonnull));

/* Return 1 if the element_name exists */
unsigned int OS_ElementExist(const OS_XML *_lxml, const char **element_name) __attribute__((nonnull));

/* Return the elements "children" of the element_name */
char **OS_GetElements(const OS_XML *_lxml, const char **element_name) __attribute__((nonnull(1)));

/* Return the elements "children" of the element_name */
xml_node **OS_GetElementsbyNode(const OS_XML *_lxml, const xml_node *node) __attribute__((nonnull(1)));

/* Return the attributes of the element name */
char **OS_GetAttributes(const OS_XML *_lxml, const char **element_name) __attribute__((nonnull(1)));

/* Return one value from element_name */
char *OS_GetOneContentforElement(OS_XML *_lxml, const char **element_name) __attribute__((nonnull));

/* Return an array with the content of all entries of element_name */
char **OS_GetElementContent(OS_XML *_lxml, const char **element_name) __attribute__((nonnull));

/* Return an array with the contents of an element_nane */
char **OS_GetContents(OS_XML *_lxml, const char **element_name) __attribute__((nonnull(1)));

/* Return the value of a specific attribute of the element_name */
char *OS_GetAttributeContent(OS_XML *_lxml, const char **element_name,
                             const char *attribute_name) __attribute__((nonnull(1, 2)));

/* Apply the variables to the xml */
int OS_ApplyVariables(OS_XML *_lxml) __attribute__((nonnull));

/* Error from writer */
#define XMLW_ERROR              006
#define XMLW_NOIN               007
#define XMLW_NOOUT              010

/* Write an XML file, based on the input and values to change */
int OS_WriteXML(const char *infile, const char *outfile, const char **nodes,
                const char *oldval, const char *newval) __attribute__((nonnull(1, 2, 3, 5)));

/**
 * @brief Get value of an attribute of a node
 * @param node node to find value of attribute
 * @param name name of the attribute
 * @return value of attribute on success. NULL otherwise
 */
const char * w_get_attr_val_by_name(xml_node * node, const char * name);

/*----------------------------------------*/
/*Config file manipulation function*/
/*----------------------------------------*/

/**
 * @brief Read the module config of config file
 * @param modules modules defined in config file that you want to read config
 * @param cfgfile path to config file
 * @param d1 d2 pointer to struct, function to store value read from config.
 * @return 0 on success. -1 for failure
 */
int ReadConfig(int modules,const char *cfgfile, void *d1,void *d2);
/**
 * @brief helper function to read pub/sub configuration in config file
 * @param node xml node 
 * @param d1,d2 pointer to struct, function to store value read from config.
 * @return 0 on success. -1 for failure
 */
//int Read_Pub_Sub(XML_NODE node, void *d1, __attribute__((unused)) void *d2);
/**
 * @brief helper function to read root tag in config file
 * @param node xml node 
 * @param d1,d2 pointer to struct, function to store value read from config.
 * @return 0 on success. -1 for failure
 */
//int read_main_elements(const OS_XML *xml,int modules,XML_NODE node,void *d1,void *d2);
/**
 * @brief change config value of existing xml path
 * @param cfgfile path to config file
 * @param xmp_path path to a specific xml tag in file config
 * @param newval new value to replace
 * @return 0 on success. -1 for failure
 */
int change_config_value(const char* cfgfile,const char **xml_path,char *newval);
/**
 * @brief change config value of existing xml path
 * @param cfgfile path to config file
 * @param xmp_path path to a specific xml tag in file config
 * @param oldval old value that need to be replaced
 * @param newval new value to replace
 * @return 0 on success. -1 for failure
 */
int change_config_value_rep(const char* cfgfile,const char **xml_path,char *oldval,char*newval);

/*Helper function to cut off redundent stuffs of string*/
char *trimString(char *str);
/*Helper function to count spaces in a line*/
int count_spaces(const char *line);
/*Process need to be done before write to file */
char *prewrite_to_file(char buf[],int numspaces,const char *topic);
/*Actual function to write to config file*/
int Write_topic_to_conf(const char *infile, const char *outfile, char *topic,int module);

/**
 * @brief add new topic to xml file
 * @param cfgfile path to config file
 * @param topic topic to add to config file
 * @param module module sub or pub that you want to add config file to
 * @return 1 on success. 0 for failure
 */
int add_new_topic(const char* cfgfile,char *topic,int module);
/*Helper function help to  get content of <topic> tag*/
char *get_topic_content(char xml[]);
/*Helper function help to check if a tag is topic or not*/
bool check_tag_is_topic(char xml[]);
/*Helpter function to delete a topic from config file */
int Delete_topic(const char *infile, const char *outfile, const char *topic);
/**
 * @brief delete a topic from xml file
 * @param topic topic to delete to config file
 * @return 1 on success. 0 for failure
 */
int delete_topic_from_conf(const char* cfgfile,char *topic);


#endif /* OS_XML_H */
