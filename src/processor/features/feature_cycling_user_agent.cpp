#include <string.h>
#include "../utils/hashfunctions/crc32.h"
#include "../log_entry.h"
#include "feature_container.h"
#include "feature_cycling_user_agent.h"


struct cuaEntry 
{
	unsigned int hash;
	int counter;
};
struct cuaData
{
	struct cuaEntry userAgents[MAX_USER_AGENTS];
	int numAgents;
	int maxRequests;
};

int FeatureCyclingUserAgent::GetDataSize()
{
	return sizeof(cuaData);
}
void FeatureCyclingUserAgent::Aggregrate(LogEntry *le,FeatureContainer *fc,void *data,double *featureValue)
{
	cuaData *myData=(cuaData *) data;

	//sha1(le->userAgent);
	//sha1::calc(le->userAgent,strlen(le->userAgent),hash);
	unsigned int crc32=Crc32_ComputeBuf(0,le->userAgent,strlen(le->userAgent));
	unsigned int *hashValue=&crc32;
	int curVal=0;
	int i;
	for(i=1;i<=myData->numAgents;i++) //0 = other
	{
		if(myData->userAgents[i].hash==*hashValue)
		{
			curVal=++(myData->userAgents[i].counter);
			break;
		}
	}
	if (i>myData->numAgents)
	{
		if (i==MAX_USER_AGENTS )
		{
			// do not count the overflow as max
			++(myData->userAgents[0].counter);

		}
		else
		{
			myData->numAgents++;
			myData->userAgents[myData->numAgents].hash=*hashValue;
			curVal=myData->userAgents[myData->numAgents].counter=1;
			
		}

	}
	myData->maxRequests=max(curVal,myData->maxRequests);
	*featureValue=((double) myData->maxRequests)/((double)fc->numrequests);
	
	
}
