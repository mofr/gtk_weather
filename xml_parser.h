#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

std::string parseTermo (xmlDocPtr doc, xmlNodePtr cur) {
    std::string result;
	xmlChar *prop;
	cur = cur->xmlChildrenNode;

    while(cur!=NULL)
    {
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"avg"))) {
       prop=xmlGetProp(cur,(xmlChar*)"temp");
       result=" (";
       result+=(char*)prop;
       result+=") °C";
       xmlFree(prop);
 	   }
    cur=cur->next;
    }

    return result;
}

std::string parseCurrentTermo (xmlDocPtr doc, xmlNodePtr cur, char *change) {
    std::string result;
	xmlChar *prop;
//	cur = cur->xmlChildrenNode;

    while(cur!=NULL)
    {
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"current"))) {
       prop=xmlGetProp(cur,(xmlChar*)"date");
       result+=(char*)prop;
       result+=" ";
       xmlFree(prop);
       prop=xmlGetProp(cur,(xmlChar*)"time");
       result+=(char*)prop;
       result+="\n";
       xmlFree(prop);

       prop=xmlGetProp(cur,(xmlChar*)"temp");
       result+=(char*)prop;
       xmlFree(prop);
       prop=xmlGetProp(cur,(xmlChar*)"change");
       *change=prop[0];
       xmlFree(prop);
 	   }
    cur=cur->next;
    }

    return result;
}

std::string parseDoc(std::string doctext, char*change) {
    std::string result;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseMemory(doctext.c_str(),doctext.length());
	
	if (doc == NULL ) {
		printf("Document not parsed successfully.\n");
		return result;
	}
	
	cur = xmlDocGetRootElement(doc);
	
	if (cur == NULL) {
		printf("empty document\n");
		xmlFreeDoc(doc);
		return result;
	}
	
	if (xmlStrcmp(cur->name, (const xmlChar *) "termo")) {
		printf("document of the wrong type, root node != termo\n");
		xmlFreeDoc(doc);
		return result;
	}
	
	cur = cur->xmlChildrenNode;
    result=parseCurrentTermo(doc,cur,change);

    while(cur!=NULL)
    {
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"day")))
       {
//       result+="За день:\n";
	   result+=parseTermo (doc, cur);
	   }/*
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"week")))
       {
       result+="За неделю:\n";
	   result+=parseTermo (doc, cur);
	   }
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"month")))
       {
       result+="За месяц:\nrtrty";
	   result+=parseTermo (doc, cur);
	   }*/
    cur = cur->next;
    }
	
	xmlFreeDoc(doc);
	return result;
}
