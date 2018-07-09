// uglyfont.cpp
//
// Copyright: Soji Yamakawa (CaptainYS, E-Mail: PEB01130*nifty+com  <- Replace * with @, + with .)
//
// I don't abandon the copyright, but you can use this code and the header 
// (uglyfont.cpp and uglyfont.h) for your product regardless of the purpose,
// i.e., free or commercial, open source or proprietary.
//
// However, I do not take any responsibility for the consequence of using
// this code and header.  Please use on your own risks.
//
// January 5, 2005
//
// Soji Yamakawa

#include <stdio.h>
#include <string.h>
#include "delfem/uglyfont.h"


#if defined(_WIN32)
#  include <windows.h>
#if defined(__VISUALC__)
#  pragma comment (lib, "winmm.lib")      /* link with Windows MultiMedia lib */
#  pragma comment (lib, "opengl32.lib")  /* link with Microsoft OpenGL lib */
#  pragma comment (lib, "glu32.lib")     /* link with Microsoft OpenGL Utility lib */
#endif
#endif  /* _WIN32 */

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif


// The following integer arrays define the ugly font geometry
// Coordinate ranges in the arrays are 0<=x<=100 and 0<=y<=100.
const double YsUglyFontWid=100.0;
const double YsUglyFontHei=100.0;


static int Ptn032[]={   /*   */
32,
-1
};
static int Ptn033[]={   /* ! */
33,
0,3,50,100,75,100,50,25,
0,4,50,16,62,16,62,0,50,0,
-1
};
static int Ptn034[]={   /* " */
34,
2,4,37,100,37,83,62,100,62,83,
-1
};
static int Ptn035[]={   /* # */
35,
2,8,12,66,87,66,12,33,87,33,37,91,37,8,62,8,62,91,
-1
};
static int Ptn036[]={   /* $ */
36,
1,12,87,75,75,83,25,83,12,75,12,58,25,50,75,50,87,41,87,25,75,16,25,16,12,25,
2,4,37,91,37,8,62,8,62,91,
-1
};
static int Ptn037[]={   /* % */
37,
1,2,87,100,12,0,
1,5,12,100,37,100,37,75,12,75,12,100,
1,5,87,0,87,25,62,25,62,0,87,0,
-1
};
static int Ptn038[]={   /* & */
38,
1,11,87,33,62,0,25,0,0,16,0,41,75,83,75,91,62,100,37,100,25,83,87,0,
-1
};
static int Ptn039[]={   /* ' */
39,
0,3,50,83,50,100,62,100,
-1
};
static int Ptn040[]={   /* ( */
40,
1,6,62,100,37,83,25,58,25,41,37,16,62,0,
-1
};
static int Ptn041[]={   /* ) */
41,
1,6,37,100,62,83,75,58,75,41,62,16,37,0,
-1
};
static int Ptn042[]={   /* * */
42,
2,8,50,100,50,0,0,50,100,50,87,91,12,8,87,8,12,91,
-1
};
static int Ptn043[]={   /* + */
43,
2,4,12,50,87,50,50,75,50,25,
-1
};
static int Ptn044[]={   /* , */
44,
1,4,37,25,62,25,62,8,37,-8,
-1
};
static int Ptn045[]={   /* - */
45,
1,2,12,50,87,50,
-1
};
static int Ptn046[]={   /* . */
46,
0,4,37,16,62,16,62,0,37,0,
-1
};
static int Ptn047[]={   /* / */
47,
1,2,100,100,0,0,
-1
};
static int Ptn048[]={   /* 0 */
48,
1,9,25,100,75,100,100,83,100,16,75,0,25,0,0,16,0,83,25,100,
1,2,87,91,12,8,
-1
};
static int Ptn049[]={   /* 1 */
49,
1,3,25,83,50,100,50,0,
-1
};
static int Ptn050[]={   /* 2 */
50,
1,8,12,83,37,100,75,100,100,83,100,66,12,16,12,0,100,0,
-1
};
static int Ptn051[]={   /* 3 */
51,
1,11,12,83,37,100,75,100,100,83,100,66,75,50,100,33,100,16,75,0,25,0,0,16,
-1
};
static int Ptn052[]={   /* 4 */
52,
1,3,37,100,12,25,87,25,
1,2,62,75,62,0,
-1
};
static int Ptn053[]={   /* 5 */
53,
1,10,87,100,12,100,12,41,37,58,62,58,87,41,87,16,62,0,37,0,12,16,
-1
};
static int Ptn054[]={   /* 6 */
54,
1,12,87,83,62,100,25,100,0,83,0,16,25,0,75,0,100,16,100,33,75,50,25,50,0,33,
-1
};
static int Ptn055[]={   /* 7 */
55,
1,5,12,83,12,100,87,100,50,33,50,0,
-1
};
static int Ptn056[]={   /* 8 */
56,
1,9,100,83,75,100,25,100,0,83,0,66,25,50,75,50,100,66,100,83,
1,8,25,50,0,33,0,16,25,0,75,0,100,16,100,33,75,50,
-1
};
static int Ptn057[]={   /* 9 */
57,
1,12,0,16,25,0,75,0,100,16,100,83,75,100,25,100,0,83,0,58,25,41,75,41,100,58,
-1
};
static int Ptn058[]={   /* : */
58,
0,4,37,91,62,91,62,75,37,75,
0,4,37,25,62,25,62,8,37,8,
-1
};
static int Ptn059[]={   /* ; */
59,
0,4,37,91,62,91,62,75,37,75,
0,4,37,25,62,25,62,8,37,8,
1,2,62,8,37,-8,
-1
};
static int Ptn060[]={   /* < */
60,
1,3,87,100,12,50,87,0,
-1
};
static int Ptn061[]={   /* = */
61,
2,4,12,66,87,66,12,33,87,33,
-1
};
static int Ptn062[]={   /* > */
62,
1,3,12,0,87,50,12,100,
-1
};
static int Ptn063[]={   /* ? */
63,
1,8,12,83,37,100,75,100,100,83,100,66,75,50,50,50,50,25,
0,4,50,16,62,16,62,8,50,8,
-1
};
static int Ptn064[]={   /* @ */
64,
1,18,62,50,50,58,25,58,12,41,12,25,25,16,50,16,62,41,75,25,87,66,75,91,62,100,25,100,0,75,0,16,25,0,62,0,87,16,
-1
};
static int Ptn065[]={   /* A */
65,
1,3,0,0,50,100,100,0,
1,2,25,50,75,50,
-1
};
static int Ptn066[]={   /* B */
66,
1,10,0,0,0,100,75,100,87,91,87,58,75,50,100,33,100,8,87,0,0,0,
1,2,75,50,0,50,
-1
};
static int Ptn067[]={   /* C */
67,
1,8,100,83,75,100,25,100,0,83,0,16,25,0,75,0,100,16,
-1
};
static int Ptn068[]={   /* D */
68,
1,7,0,100,75,100,100,83,100,16,75,0,0,0,0,100,
-1
};
static int Ptn069[]={   /* E */
69,
1,4,100,100,0,100,0,0,100,0,
1,2,0,50,87,50,
-1
};
static int Ptn070[]={   /* F */
70,
1,3,100,100,0,100,0,0,
1,2,0,50,75,50,
-1
};
static int Ptn071[]={   /* G */
71,
1,10,100,83,75,100,25,100,0,83,0,16,25,0,75,0,100,16,100,41,62,41,
-1
};
static int Ptn072[]={   /* H */
72,
2,6,0,100,0,0,100,100,100,0,0,50,100,50,
-1
};
static int Ptn073[]={   /* I */
73,
2,6,37,100,62,100,37,0,62,0,50,0,50,100,
-1
};
static int Ptn074[]={   /* J */
74,
1,2,75,100,100,100,
1,6,87,100,87,16,62,0,37,0,12,16,12,33,
-1
};
static int Ptn075[]={   /* K */
75,
1,2,12,100,12,0,
1,2,12,33,100,100,
1,2,25,41,100,0,
-1
};
static int Ptn076[]={   /* L */
76,
1,3,0,100,0,0,100,0,
-1
};
static int Ptn077[]={   /* M */
77,
1,5,0,0,0,100,50,50,100,100,100,0,
-1
};
static int Ptn078[]={   /* N */
78,
1,4,0,0,0,100,100,0,100,100,
-1
};
static int Ptn079[]={   /* O */
79,
1,9,0,83,25,100,75,100,100,83,100,16,75,0,25,0,0,16,0,83,
-1
};
static int Ptn080[]={   /* P */
80,
1,7,0,0,0,100,75,100,100,83,100,66,75,50,0,50,
-1
};
static int Ptn081[]={   /* Q */
81,
1,9,25,0,0,16,0,83,25,100,75,100,100,83,100,16,75,0,25,0,
1,2,62,25,100,0,
-1
};
static int Ptn082[]={   /* R */
82,
1,7,0,0,0,100,75,100,100,83,100,66,75,50,0,50,
1,3,75,50,100,33,100,0,
-1
};
static int Ptn083[]={   /* S */
83,
1,12,100,83,75,100,25,100,0,83,0,66,25,50,75,50,100,33,100,16,75,0,25,0,0,16,
-1
};
static int Ptn084[]={   /* T */
84,
2,4,0,100,100,100,50,100,50,0,
-1
};
static int Ptn085[]={   /* U */
85,
1,6,0,100,0,16,25,0,75,0,100,16,100,100,
-1
};
static int Ptn086[]={   /* V */
86,
1,3,0,100,50,0,100,100,
-1
};
static int Ptn087[]={   /* W */
87,
1,5,0,100,25,0,50,66,75,0,100,100,
-1
};
static int Ptn088[]={   /* X */
88,
2,4,0,0,100,100,100,0,0,100,
-1
};
static int Ptn089[]={   /* Y */
89,
1,3,0,100,50,50,50,0,
1,2,50,50,100,100,
-1
};
static int Ptn090[]={   /* Z */
90,
1,4,0,100,100,100,0,0,100,0,
-1
};
static int Ptn091[]={   /* [ */
91,
1,4,62,100,37,100,37,0,62,0,
-1
};
static int Ptn092[]={   /* \ */
92,
1,2,0,100,100,0,
-1
};
static int Ptn093[]={   /* ] */
93,
1,4,37,100,62,100,62,0,37,0,
-1
};
static int Ptn094[]={   /* ^ */
94,
1,3,0,66,50,91,100,66,
-1
};
static int Ptn095[]={   /* _ */
95,
1,2,0,8,100,8,
-1
};
static int Ptn096[]={   /* ` */
96,
0,3,37,100,50,100,50,83,
-1
};
static int Ptn097[]={   /* a */
97,
1,5,12,50,25,58,75,58,87,50,87,0,
1,7,87,33,25,33,12,25,12,8,25,0,75,0,87,8,
-1
};
static int Ptn098[]={   /* b */
98,
1,7,12,100,12,0,75,0,87,8,87,50,75,58,12,58,
-1
};
static int Ptn099[]={   /* c */
99,
1,8,87,50,75,58,25,58,12,50,12,8,25,0,75,0,87,8,
-1
};
static int Ptn100[]={   /* d */
100,
1,8,87,100,87,0,25,0,12,8,12,50,25,58,75,58,87,50,
-1
};
static int Ptn101[]={   /* e */
101,
1,10,12,33,87,33,87,50,75,58,25,58,12,50,12,8,25,0,75,0,87,8,
-1
};
static int Ptn102[]={   /* f */
102,
1,4,75,100,62,100,50,91,50,0,
1,2,25,58,75,58,
-1
};
static int Ptn103[]={   /* g */
103,
1,5,87,58,87,0,75,-8,25,-8,12,0,
1,8,87,50,75,58,25,58,12,50,12,33,25,25,75,25,87,33,
-1
};
static int Ptn104[]={   /* h */
104,
1,2,12,0,12,100,
1,5,12,50,25,58,75,58,87,50,87,0,
-1
};
static int Ptn105[]={   /* i */
105,
1,2,50,75,50,66,
1,2,50,58,50,0,
-1
};
static int Ptn106[]={   /* j */
106,
1,2,50,75,50,66,
1,4,50,58,50,0,37,-8,12,-8,
-1
};
static int Ptn107[]={   /* k */
107,
1,2,12,100,12,0,
1,3,87,0,12,33,75,58,
-1
};
static int Ptn108[]={   /* l */
108,
1,3,37,100,50,100,50,0,
-1
};
static int Ptn109[]={   /* m */
109,
1,5,12,0,12,58,75,58,87,50,87,0,
1,3,37,58,50,50,50,0,
-1
};
static int Ptn110[]={   /* n */
110,
1,5,12,0,12,58,75,58,87,50,87,0,
-1
};
static int Ptn111[]={   /* o */
111,
1,9,25,0,12,8,12,50,25,58,75,58,87,50,87,8,75,0,25,0,
-1
};
static int Ptn112[]={   /* p */
112,
1,2,12,58,12,-16,
1,7,12,50,25,58,75,58,87,50,87,8,75,0,12,0,
-1
};
static int Ptn113[]={   /* q */
113,
1,2,87,58,87,-16,
1,7,87,50,75,58,25,58,12,50,12,8,25,0,87,0,
-1
};
static int Ptn114[]={   /* r */
114,
1,2,25,58,25,0,
1,3,25,50,62,58,87,58,
-1
};
static int Ptn115[]={   /* s */
115,
1,10,87,50,75,58,25,58,12,50,12,41,87,16,87,8,75,0,25,0,12,8,
-1
};
static int Ptn116[]={   /* t */
116,
1,2,25,58,75,58,
1,4,37,75,37,8,50,0,75,0,
-1
};
static int Ptn117[]={   /* u */
117,
1,5,12,58,12,8,25,0,62,0,87,8,
1,2,87,58,87,0,
-1
};
static int Ptn118[]={   /* v */
118,
1,3,12,58,50,0,87,58,
-1
};
static int Ptn119[]={   /* w */
119,
1,5,12,58,25,0,50,41,75,0,87,58,
-1
};
static int Ptn120[]={   /* x */
120,
2,4,87,0,12,58,87,58,12,0,
-1
};
static int Ptn121[]={   /* y */
121,
2,4,87,58,12,-25,12,58,50,16,
-1
};
static int Ptn122[]={   /* z */
122,
1,4,12,58,87,58,12,0,87,0,
-1
};
static int Ptn123[]={   /* { */
123,
1,6,75,100,50,100,37,91,37,8,50,0,75,0,
1,2,37,50,25,50,
-1
};
static int Ptn124[]={   /* | */
124,
1,2,50,100,50,0,
-1
};
static int Ptn125[]={   /* } */
125,
1,6,25,0,50,0,62,8,62,91,50,100,25,100,
1,2,62,50,75,50,
-1
};
static int Ptn126[]={   /* ~ */
126,
1,2,0,91,100,91,
-1
};

static int *YsUglyFontSet[]=
{
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
Ptn032,Ptn033,Ptn034,Ptn035,Ptn036,Ptn037,Ptn038,Ptn039,Ptn040,Ptn041,Ptn042,Ptn043,Ptn044,Ptn045,Ptn046,Ptn047,
Ptn048,Ptn049,Ptn050,Ptn051,Ptn052,Ptn053,Ptn054,Ptn055,Ptn056,Ptn057,Ptn058,Ptn059,Ptn060,Ptn061,Ptn062,Ptn063,
Ptn064,Ptn065,Ptn066,Ptn067,Ptn068,Ptn069,Ptn070,Ptn071,Ptn072,Ptn073,Ptn074,Ptn075,Ptn076,Ptn077,Ptn078,Ptn079,
Ptn080,Ptn081,Ptn082,Ptn083,Ptn084,Ptn085,Ptn086,Ptn087,Ptn088,Ptn089,Ptn090,Ptn091,Ptn092,Ptn093,Ptn094,Ptn095,
Ptn096,Ptn097,Ptn098,Ptn099,Ptn100,Ptn101,Ptn102,Ptn103,Ptn104,Ptn105,Ptn106,Ptn107,Ptn108,Ptn109,Ptn110,Ptn111,
Ptn112,Ptn113,Ptn114,Ptn115,Ptn116,Ptn117,Ptn118,Ptn119,Ptn120,Ptn121,Ptn122,Ptn123,Ptn124,Ptn125,Ptn126,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,
NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  ,NULL  
};








const int YsUglyFontBase=1400;

static inline void YsDrawUglyFontPattern(int *ptn)
{
	if(ptn!=NULL)
	{
		int j,*ptr;
		ptr=ptn;
		ptr++;  // Skip charactor code
		while(ptr[0]!=-1)
		{
			switch(ptr[0])
			{
			default:
			case 0:
				glBegin(GL_POLYGON);
				break;
			case 1:
				glBegin(GL_LINE_STRIP);
				break;
			case 2:
				glBegin(GL_LINES);
				break;
			}

			for(j=0; j<ptr[1]; j++)
			{
				glVertex2i(ptr[2+j*2],ptr[3+j*2]);
			}

			glEnd();

			ptr=ptr+2+ptr[1]*2;
		}
	}
	glTranslated(YsUglyFontWid*8.0/7.0,0.0,0.0);
}

static inline void YsMakeUglyFontDisplayList(void)
{
	if(glIsList(YsUglyFontBase)!=GL_TRUE)
	{
		int i;
		for(i=0; i<256; i++)
		{
			glNewList(YsUglyFontBase+i,GL_COMPILE);
			YsDrawUglyFontPattern(YsUglyFontSet[i]);
			glEndList();
		}
	}
}

void YsDrawUglyFont(const char str[],int centering,int useDisplayList)
{
	int l;

	l=strlen(str);

	glPushMatrix();

	if(centering)
	{
		glTranslated(-(double)l/2.0,-0.5,0);
	}

	glScaled(1.0/(YsUglyFontWid*8.0/7.0),1.0/YsUglyFontHei,1.0);

	if(useDisplayList)
	{
		YsMakeUglyFontDisplayList();
		glPushAttrib(GL_LIST_BIT);
		glListBase(YsUglyFontBase);
		glCallLists(l,GL_UNSIGNED_BYTE,str);
		glPopAttrib();
	}
	else
	{
		int i;
		for(i=0; str[i]!=0; i++)
		{
			YsDrawUglyFontPattern(YsUglyFontSet[((unsigned char *)str)[i]]);
		}
	}

	glPopMatrix();
}
