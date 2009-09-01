/*
---------------------------------------------------------------------------
Copyright (C) 2003  Eugenio Cervesato & Giorgio De Odorico.
Developed at the Associazione per la Ricerca in Cardiologia - Pordenone - Italy.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
---------------------------------------------------------------------------
*/
// codes.h             consts included in the protocol
#ifndef __CODES_H__
#define __CODES_H__

static const char STR_END[]={-1,'\0'};
static char STR_NULL[]=" unspecified/unknown ";

static alfabetic _month[]={
    { 0	," unspecified/unknown "},
    {	 1	," January "},
    {	 2	," February "},
{	 3	," March "},
{	 4	," April "},
{	 5	," May "},
{	 6	," June "},
{	 7	," July "},
{	 8	," August "},
{	 9	," September "},
{	10	," October "},
{	11	," November "},
{	12	," December "}
};

static alfabetic _special[]={
    {	29999	,"measurements not computed"},
    {	29998	,"measurements not found due to rejection of the lead"},
    {	19999	,"measurements not found because wave not present"},
    {	  999	,"undefined"}
};

static alfabetic class_drug[]={
    {	   0	, " unspecified/unknown "} ,
    {	   1	, " digitalis preparation "} ,
    {	   2	, " antiarrhythmic "} ,
    {	   3	, " diuretics "} ,
    {	   4	, " antihypertensive "} ,
    {	   5	, " antianginal "} ,
    {	   6	, " antithrombotic agents "} ,
    {	   7	, " beta blockers "} ,
    {	   8	, " psychotropic "} ,
    {	   9	, " calcium blockers "} ,
    {	  10	, " antihypotensive "} ,
    {	  11	, " anticholesterol "} ,
    {	  12	, " ACE-inhibitors "} ,
    {	 100	, " not taking drugs "} ,
    {	 101	, " drugs, but unknown type "} ,
    {	 102	, " other medication "}, 
    {	 256	, " unspecified/unknown "} ,
    {	 257	, " digoxin-lanoxin "} ,
    {	 258	, " digitoxin-digitalis "} ,
    {	 265	, " other "},
    {	 512	, " unspecified/unknown "} ,
    {	 513	, " dysopyramide "} ,
    {	 514	, " quinidine "} ,
    {	 515	, " procainamide "} ,
    {	 516	, " lidocaine "} ,
    {	 517	, " phenytoin "} ,
    {	 518	, " dilantin "} ,
    {	 519	, " amiodarone "} ,
    {	 520	, " tocainide "} ,
    {	 521	, " other "} ,
    {	 522	, " encainide "} ,
    {	 523	, " mexitil/mexilitine "} ,
    {	 524	, " flecainide "} ,
    {	 525	, " lorcainide "} ,
    {	 768	, " unspecified/unknown "} ,
    {	 769	, " thiazide "} ,
    {	 770	, " furosemide (lasix) "} ,
    {	 771	, " potassium cloride "} ,
    {	 777	, " other "} ,
    {	1024	, " unspecified/unknown "} ,
    {	1025	, " clonidine "} ,
    {	1026	, " prasozin "} ,
    {	1027	, " hydralazine "} ,
    {	1033	, " other "},
    {	1280	, " unspecified/unknown "} ,
    {	1281	, " isosorbide "} ,
    {	1282	, " calcium blockers "} ,
    {	1283	, " diuretics "} ,
    {	1284	, " nitrates "} ,
    {	1289	, " other "},
    {	1536	, " unspecified/unknown "} ,
    {	1537	, " aspirin "} ,
    {	1538	, " coumarin "} ,
    {	1539	, " heparin "} ,
    {	1540	, " warfarin "} ,
    {	1541	, " streptokinase "} ,
    {	1542	, " t-PA "} ,
    {	1545	, " other "},
    {	1792	, " unspecified/unknown "} ,
    {	1793	, " propanolol "} ,
    {	1794	, " corgard "} ,
    {	1795	, " atenolol "} ,
    {	1796	, " metoprolol "} ,
    {	1797	, " pindolol "} ,
    {	1798	, " acebutolol "} ,
    {	1801	, " other "}, 
    {	2048	, " unspecified/unknown "} ,
    {	2049	, " tricyclic antidepressant "} ,
    {	2050	, " phenothiazide "} ,
    {	2051	, " barbiturate "} ,
    {	2057	, " other "},
    {	2304	, " unspecified/unknown "} ,
    {	2305	, " nifedipine "} ,
    {	2306	, " verapamil "} ,
    {	2313	, " other "},
    {	2560	, " unspecified/unknown "} ,
    {	2561	, " asthmatic drug "} ,
    {	2562	, " aminophyline "} ,
    {	2563	, " isuprel "} ,
    {	2569	, " other "},
    {	2816	, " unspecified/unknown "} ,
    {	2817	, " colestid "} ,
    {	2818	, " lovastatin "} ,
    {	2819	, " simvastatin "} ,
    {	2820	, " fibrates "} ,
        {	2825	, " other "},
    {	3071	, " unspecified/unknown "} ,
    {	3072	, " captopril "} ,
        {	3081	, " other "} 
};

static alfabetic compatibility[]={
    {	72	, " I "} ,
    {	160	, " II "} ,
    {   176	, " III "} ,
    {	192	, " IV "},
    {	255	, " unspecified/unknown "}
};

static alfabetic language_code[]={
    {	  0	, " 8 bit ASCII only "} ,
    {	  1	, " ISO-8859-1 latin-1 "} ,
    {	192	, " ISO-8859-2 latin-2 (central and estern european) "} ,
    {	208	, " ISO-8859-4 latin-4 (Baltic) "} ,
    {	200	, " ISO-8859-5 (Cyrillic) "} ,
    {	216	, " ISO-8859-6 (Arabic) "} ,
    {	196	, " ISO-8859-7 (Greek) "} ,
    {	212	, " ISO-8859-8 (Hebrew) "} ,
    {	204	, " ISO-8859-11 (Thai) "} ,
    {	220	, " ISO-8859-15 latin-9 (latin-0) "} ,
    {	224	, " Unicode (ISO-60646) "} ,
    {	240	, " JIS X0201-1976 (Japanese) "} ,
    {	232	, " JIS X0208-1977 (Japanese) "} ,
    {	248	, " JIS X0212-1990 (Japanese) "} ,
    {	228	, " GB 2312-80 (Chinese) "} ,
    {	244	, " KS C5601-1987 (Korean) "} ,
    {	255	, " unspecified/unknown "} 
};

static alfabetic _hystory[]={
    {  0	, " diagnoses or clinical problems "} ,
    {	  1	, " apparently healty "} ,
    {	 10	, " acute myocardial infarction "} ,
    {	 11	, " myocardial infarction "} ,
    {	 12	, " previous myocardial infarction "} ,
    {	 15	, " ischemic heart disease "} ,
    {	 18	, " peripheral vascular disease "} ,
    {	 20	, " cyanotic congenital heart disease "} ,
    {	 21	, " acyanotic congenital heart disease "} ,
    {	 22	, " valvular heart disease "} ,
    {	 25	, " hypertension "} ,
    {	 27	, " cerebrovascular accident "} ,
    {	 30	, " cardiomyopathy "} ,
    {	 35	, " pericardits "} ,
    {	 36	, " myocardits "} ,
    {	 40	, " post-operative cardiac surgery "} ,
    {	 42	, " implanted cardiac pacemaker "} ,
    {	 45	, " pulmonary embolism "} ,
    {	 50	, " respiratory disease "} ,
    {	 55	, " endocrine disease "} ,
    {	 60	, " neurological disease "} ,
    {	 65	, " alimentary disease "} ,
    {	 70	, " renal disease "} ,
    {	 80	, " pre-operative general surgery "} ,
    {	 81	, " post-operative general surgery "} ,
    {	 90	, " general medical "} ,
    {	100	, " unspecified/unknown "} 
};

#endif /*__CODES_H__*/
