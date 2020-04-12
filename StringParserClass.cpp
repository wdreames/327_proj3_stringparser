/*
 * StringParserClass.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: William Reames
 */

#include <string>
#include <string.h>
#include <iostream>

#include "../327_proj3_test/includes/StringParserClass.h"
#include "../327_proj3_test/includes/constants.h"

using namespace std;
using namespace KP_StringParserClass;

//initializes member variables
StringParserClass::StringParserClass(void): pStartTag(0), pEndTag(0){
	areTagsSet = false;
}

//calls cleanup to release any allocated memory
StringParserClass::~StringParserClass(void){
	cleanup();
}

//these are the start tag and the end tags that we want to find,
//presumably the data of interest is between them, please make a
//COPY of what pStartTag and pEndTag point to.  In other words
//DO NOT SET pStartTag = pStart
//returns:
//SUCCESS
//ERROR_TAGS_NULL if either pStart or pEnd is null
int StringParserClass::setTags(const char *pStart, const char *pEnd){
	if(pStart && pEnd){
		//makes sure pStartTag and pEndTag don't have extraneous data
		cleanup();

		//makes a deep copy of the start tag
		int startLen = strlen(pStart)+1;
		pStartTag = new char[startLen];
		strncpy(pStartTag, pStart, startLen);

		//makes a deep copy of the end tag
		int endLen = strlen(pEnd)+1;
		pEndTag = new char[endLen];
		strncpy(pEndTag, pEnd, endLen);

		areTagsSet = true;
		return SUCCESS;
	}
	else{
		return ERROR_TAGS_NULL;
	}
}

//First clears myVector
//going to search thru pDataToSearchThru, looking for info bracketed by
//pStartTag and pEndTag, will add that info only to myVector
//returns
//SUCCESS  finished searching for data between tags, results in myVector (0 or more entries)
//ERROR_TAGS_NULL if either pStart or pEnd is null
//ERROR_DATA_NULL pDataToSearchThru is null
int StringParserClass::getDataBetweenTags(char *pDataToSearchThru, std::vector<std::string> &myVector){
	if(areTagsSet){
		if(pDataToSearchThru){
			myVector.clear();
			
			//copies the input data so it is not altered
			int dataLenToCopy = strlen(pDataToSearchThru)+1;
			char* dataCopy = 0;
			dataCopy = new char[dataLenToCopy];
			strncpy(dataCopy, pDataToSearchThru, dataLenToCopy);

			//boolean variable used to determine when to stop looping
			bool reached_end = false;

			while(!reached_end){
				int dataLen = strlen(dataCopy)+1;

				//copies dataCopy into a new pointer used to find the startTag
				char *beginStartTag = 0;
				beginStartTag = new char[dataLen];
				strncpy(beginStartTag, dataCopy, dataLen);

				//copies dataCopy into a new pointer used to find the startTag
				char *endStartTag = 0;
				endStartTag = new char[dataLen];
				strncpy(endStartTag, dataCopy, dataLen);

				//Finds the start tag
				if(findTag(pStartTag, beginStartTag, endStartTag) != 0){
					//if a SUCCESS was not returned, the loop will stop
					reached_end = true;
				}
				else{
					//cuts dataCopy where the start tag was found
					//this makes the program search for an end tag *after* the start tag
					int cutDataLen = strlen(endStartTag+1)+1;

					//creates a new pointer used to find the endTag
					char *beginEndTag = 0;
					beginEndTag = new char[cutDataLen];
					strncpy(beginEndTag, endStartTag+1, cutDataLen);

					//creates a new pointer used to find the endTag
					char *endEndTag = 0;
					endEndTag = new char[cutDataLen];
					strncpy(endEndTag, endStartTag+1, cutDataLen);

					//Finds the end tag
					if(findTag(pEndTag, beginEndTag, endEndTag) != 0){
						//if a SUCCESS was not returned, the loop will stop
						reached_end = true;
					}
					else{
						//Cuts out the data between the tags and adds to the output vector

						string output = string(endStartTag+1);
						string cutFromOutput = string(beginEndTag);
						string::size_type i = output.find(cutFromOutput);
						if (i != std::string::npos)
						   output.erase(i, cutFromOutput.length());
						myVector.push_back(output);

						//changes dataCopy so the next iteration will be searching *after* the last end tag
						if(dataCopy){
							delete[] dataCopy;
							dataCopy = 0;
						}
						int len = strlen(endEndTag+1)+1;
						if(len <= 1){
							//Ends if the new data set has a length of 0
							reached_end = true;
						}
						else{
							dataCopy = new char[len];
							strncpy(dataCopy, endEndTag+1, len);
						}
					}

					//memory management
					if(endEndTag){
						delete [] endEndTag;
						endEndTag = 0;
					}
					if(beginEndTag){
						delete [] beginEndTag;
						beginEndTag = 0;
					}
				}

				//memory management
				if(endStartTag){
					delete [] endStartTag;
					endStartTag = 0;
				}
				if(beginStartTag){
					delete [] beginStartTag;
					beginStartTag = 0;
				}
			}
			//memory management
			if(dataCopy){
				delete[] dataCopy;
				dataCopy = 0;
			}
			return SUCCESS;
		}
		else{
			return ERROR_DATA_NULL;
		}
	}
	else{
		return ERROR_TAGS_NULL;
	}
}

void StringParserClass::cleanup(){
	if(pStartTag){
		delete [] pStartTag;
		pStartTag = 0;
	}
	if(pEndTag){
		delete [] pEndTag;
		pEndTag = 0;
	}
	areTagsSet = false;
}

//Searches a string starting at pStart for pTagToLookFor
//returns:
//SUCCESS  found pTagToLookFor, pStart points to beginning of tag and pEnd points to end of tag
//FAIL did not find pTagToLookFor and pEnd points to 0
//ERROR_TAGS_NULL if either pStart or pEnd is null
int StringParserClass::findTag(char *pTagToLookFor, char *&pStart, char *&pEnd){
	if(pStart == nullptr or pEnd == nullptr){
		return ERROR_TAGS_NULL;
	}

	//makes a copy of pStart that can be manipulated in the program
	int startLen = strlen(pStart)+1;
	char *tempStart = 0;
	tempStart = new char[startLen];
	strncpy(tempStart, pStart, startLen);

	//makes a copy of pEnd that can be manipulated in the program
	int endLen = strlen(pEnd)+1;
	char *tempEnd = 0;
	tempEnd = new char[endLen];
	strncpy(tempEnd, pStart, endLen);

	//lengths used for looping
	int dataLength = strlen(tempStart);
	int tagLength = strlen(pTagToLookFor);

	for(int i = 0; i < dataLength; ++i){
		//If it finds the same starting character as the tag,
		//it runs a test to see if it found the whole tag or if it
		//just matched a single character
		if(pTagToLookFor[0] == tempStart[i]){
			int count = 0;
			//Starts at 1 because we already checked 0
			for(int currentTagChar = 1; currentTagChar < tagLength && i+currentTagChar < dataLength; ++currentTagChar){
				if(pTagToLookFor[currentTagChar] == tempStart[i+currentTagChar]){
					count++;
				}
				else{
					break;
				}
			}
			//Tests if the whole tag was found
			if(count == tagLength-1){

				//Deletes pStart and pEnd so their values can be replaced
				if(pStart){
					delete [] pStart;
					pStart = 0;
				}
				if(pEnd){
					delete [] pEnd;
					pEnd = 0;
				}

				//sets pStart to point to the start of the tag
				int newStartLen = strlen(tempStart + i) + 1;
				pStart = new char[newStartLen];
				strncpy(pStart, tempStart + i, newStartLen);

				//sets pEnd to point to the start of the tag
				int newEndLen = strlen(tempEnd + i + tagLength - 1) + 1;
				pEnd = new char[newEndLen];
				strncpy(pEnd, tempEnd + i + tagLength - 1, newEndLen);

				//memory management
				if(tempStart){
					delete [] tempStart;
					tempStart = 0;
				}
				if(tempEnd){
					delete [] tempEnd;
					tempEnd = 0;
				}
				return SUCCESS;
			}
		}
	}

	//memory management
	if(tempStart){
		delete [] tempStart;
		tempStart = 0;
	}
	if(tempEnd){
		delete [] tempEnd;
		tempEnd = 0;
	}

	//sets pEnd to 0 and returns FAIL
	if(pEnd){
		delete [] pEnd;
		pEnd = 0;
	}
	return FAIL;
}

