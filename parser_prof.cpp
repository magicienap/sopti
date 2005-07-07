#include <stdio.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>

#include <vector>
#include <string>


using namespace std;

struct teacher_info {
	string course_symbol;
	int group_type;
	string group_name;
	string teacher;
};

unsigned char*
convert (unsigned char *in, char *encoding)
{
	unsigned char *out;
        int ret,size,out_size,temp;
        xmlCharEncodingHandlerPtr handler;

        size = (int)strlen((const char *)in)+1; 
        out_size = size*2-1; 
        out = (unsigned char *) malloc((size_t)out_size); 

        if (out) {
                handler = xmlFindCharEncodingHandler(encoding);
                
                if (!handler) {
                        free(out);
                        out = NULL;
                }
        }
        if (out) {
                temp=size-1;
                ret = handler->input(out, &out_size, in, &temp);
                if (ret || temp-size+1) {
                        if (ret) {
                                printf("conversion wasn't successful.\n");
                        } else {
                                printf("conversion wasn't successful. converted: %i octets.\n",temp);
                        }
                        free(out);
                        out = NULL;
                } else {
                        out = (unsigned char *) realloc(out,out_size+1); 
                        out[out_size]=0; /*null terminating out*/
                        
                }
        } else {
                printf("no mem\n");
        }
        return (out);
}

void parse_group(xmlXPathContextPtr context, struct teacher_info ti)
{
	xmlXPathObjectPtr result;
	char *out;

	result = xmlXPathEvalExpression(BAD_CAST "enseignant", context);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		return;
	}
	context->node = result->nodesetval->nodeTab[0];
	xmlXPathFreeObject(result);
	
	result = xmlXPathEvalExpression(BAD_CAST "nom_enseignant", context);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		return;
	}
	//out = (char *) convert((unsigned char *) result->nodesetval->nodeTab[0]->children->content, "ISO-8859-1");
	ti.teacher = (char *) result->nodesetval->nodeTab[0]->children->content;
	//ti.teacher = out;
	//free(out);
	xmlXPathFreeObject(result);
	
	result = xmlXPathEvalExpression(BAD_CAST "prenom_enseignant", context);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		return;
	}
	if(result->nodesetval->nodeTab[0]->children) {
		ti.teacher += ", ";
		ti.teacher += (char *) result->nodesetval->nodeTab[0]->children->content;
	}
	xmlXPathFreeObject(result);

	printf("%s;%s;%c;%s\n", ti.course_symbol.c_str(), ti.group_name.c_str(), ti.group_type?'L':'C', ti.teacher.c_str());
	
	xmlXPathFreeContext(context);
}


void parse_horaire(xmlXPathContextPtr context, struct teacher_info orig_ti)
{
	xmlXPathObjectPtr result;
	int i;
	xmlChar *val;

	result = xmlXPathEvalExpression(BAD_CAST "groupe_cours", context);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		return;
	}

	for(i=0; i<result->nodesetval->nodeNr; i++) {
		struct teacher_info ti=orig_ti;
		xmlXPathContextPtr newcontext;
		val = xmlGetProp(result->nodesetval->nodeTab[i], BAD_CAST "no_groupe");
		ti.group_name = (char *) val;
		xmlFree(val);
		
		newcontext = xmlXPathNewContext(context->doc);
		newcontext->node = result->nodesetval->nodeTab[i];
		parse_group(newcontext, ti);
		
	}
	
	xmlXPathFreeObject(result);
	xmlXPathFreeContext(context);
}

void parse_course(string file)
{    
	int i;
	xmlDocPtr doc; /* the resulting document tree */
	xmlNode *root_element;
	xmlChar *val;
	struct teacher_info ti;
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	doc = xmlReadFile(file.c_str(), NULL, 0);
	if (doc == NULL) {
		fprintf(stderr, "Failed to parse %s\n", file.c_str());
		return;
	}
	
	context = xmlXPathNewContext(doc);
	
	result = xmlXPathEvalExpression(BAD_CAST "/racine/details/sigle", context);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		return;
	}
	ti.course_symbol = (char *) result->nodesetval->nodeTab[0]->children->content;
	xmlXPathFreeObject(result);
	
	result = xmlXPathEvalExpression(BAD_CAST "/racine/horaire", context);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		return;
	}

	for(i=0; i<result->nodesetval->nodeNr; i++) {
		xmlXPathContextPtr newcontext;
		val = xmlGetProp(result->nodesetval->nodeTab[i], BAD_CAST "type_cours");
		if(!xmlStrcmp(val, BAD_CAST "Cours")) {
			ti.group_type = 0;
		}
		else if (!xmlStrcmp(val, BAD_CAST "Travaux Pratiques")){
			ti.group_type = 1;
		}
		else {
			fprintf(stderr, "error: invalid course type");
			exit(1);
		}
		xmlFree(val);
		
		newcontext = xmlXPathNewContext(doc);
		newcontext->node = result->nodesetval->nodeTab[i];
		parse_horaire(newcontext, ti);
	}
	
	xmlXPathFreeObject(result);
	xmlXPathFreeContext(context);
	xmlFreeDoc(doc);
}

int main(int argc, char **argv)
{
	int i;

	LIBXML_TEST_VERSION

	if(argc < 2) {
		printf("bad argument count\n");
	}
	
	for(i=1; i<argc; i++)
		parse_course(argv[i]);

	xmlCleanupParser();
	return 0;
}
