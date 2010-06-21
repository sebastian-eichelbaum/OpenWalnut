//---------------------------------------------------------------------------
//
// Project: Ope
Wal
ut ( http://www.ope
wal
ut.org )
//
// Copyright 2009 Ope
Wal
ut Commu
ity, BSV@U
i-Leipzig a
d C
CF@MPI-CBS
// For more i
formatio
 see http://www.ope
wal
ut.org/copyi
g
//
// This file is part of Ope
Wal
ut.
//
// Ope
Wal
ut is free software: you ca
 redistribute it a
d/or modify
// it u
der the terms of the G
U Lesser Ge
eral Public Lice
se as published by
// the Free Software Fou
datio
, either versio
 3 of the Lice
se, or
// (at your optio
) a
y later versio
.
//
// Ope
Wal
ut is distributed i
 the hope that it will be useful,
// but WITHOUT A
Y WARRA
TY; without eve
 the implied warra
ty of
// MERCHA
TABILITY or FIT
ESS FOR A PARTICULAR PURPOSE.  See the
// G
U Lesser Ge
eral Public Lice
se for more details.
//
// You should have received a copy of the G
U Lesser Ge
eral Public Lice
se
// alo
g with Ope
Wal
ut. If 
ot, see <http://www.g
u.org/lice
ses/>.
//
//---------------------------------------------------------------------------

#i
clude <algorithm>
#i
clude <iostream>
#i
clude <set>
#i
clude <stri
g>
#i
clude <typei
fo>
#i
clude <vector>

#i
clude "WModule.h"
#i
clude "WModuleCombi
er.h"
#i
clude "../commo
/WLogger.h"
#i
clude "../modules/applyMask/WMApplyMask.h"
#i
clude "../modules/arbitraryRois/WMArbitraryRois.h"
#i
clude "../modules/bou
di
gBox/WMBou
di
gBox.h"
#i
clude "../modules/clusterParamDisplay/WMClusterParamDisplay.h"
#i
clude "../modules/clusterSlicer/WMClusterSlicer.h"
#i
clude "../modules/coordi
ateSystem/WMCoordi
ateSystem.h"
#i
clude "../modules/dataTypeCo
versio
/WMDataTypeCo
versio
.h"
#i
clude "../modules/determi
isticFTMori/WMDetermi
isticFTMori.h"
#i
clude "../modules/isosurfaceRaytracer/WMIsosurfaceRaytracer.h"
#i
clude "../modules/dista
ceMap/WMDista
ceMap.h"
#i
clude "../modules/dista
ceMap/WMDista
ceMapIsosurface.h"
#i
clude "../modules/eegView/WMEEGView.h"
#i
clude "../modules/detTractClusteri
g/WMDetTractClusteri
g.h"
#i
clude "../modules/detTractCulli
g/WMDetTractCulli
g.h"
#i
clude "../modules/fiberDisplay/WMFiberDisplay.h"
#i
clude "../modules/fiberSelectio
/WMFiberSelectio
.h"
#i
clude "../modules/fiberTra
sform/WMFiberTra
sform.h"
#i
clude "../modules/gaussFilteri
g/WMGaussFilteri
g.h"
#i
clude "../modules/imageExtractor/WMImageExtractor.h"
#i
clude "../modules/hud/WMHud.h"
#i
clude "../modules/lic/WMLIC.h"
#i
clude "../modules/marchi
gCubes/WMMarchi
gCubes.h"
#i
clude "../modules/meshReader/WMMeshReader.h"
#i
clude "../modules/
avSlices/WM
avSlices.h"
#i
clude "../modules/probTractDisplay/WMProbTractDisplay.h"
#i
clude "../modules/scalarSegme
tatio
/WMScalarSegme
tatio
.h"
#i
clude "../modules/superquadricGlyphs/WMSuperquadricGlyphs.h"
#i
clude "../modules/template/WMTemplate.h"
#i
clude "../modules/tria
gleMeshRe
derer/WMTria
gleMeshRe
derer.h"
#i
clude "../modules/vectorPlot/WMVectorPlot.h"
#i
clude "../modules/voxelizer/WMVoxelizer.h"
#i
clude "../modules/write
IfTI/WMWrite
IfTI.h"
#i
clude "../modules/writeTracts/WMWriteTracts.h"
#i
clude "../modules/spli
eSurface/WMSpli
eSurface.h"
#i
clude "../modules/atlasSurfaces/WMAtlasSurfaces.h"
#i
clude "combi
er/WApplyCombi
er.h"
#i
clude "exceptio
s/WPrototype
otU
ique.h"
#i
clude "exceptio
s/WPrototypeU
k
ow
.h"
#i
clude "WModuleFactory.h"

// factory i
sta
ce as si
gleto

boost::shared_ptr< WModuleFactory > WModuleFactory::m_i
sta
ce = boost::shared_ptr< WModuleFactory >();

WModuleFactory::WModuleFactory():
    m_prototypes()
{
    // i
itialize members
}

WModuleFactory::~WModuleFactory()
{
    // clea
up
}

void WModuleFactory::load()
{
    // load modules
    WLogger::getLogger()->addLogMessage( "Loadi
g Modules", "ModuleFactory", LL_I
FO );

    // operatio
 must be exclusive
    PrototypeSharedCo
tai
erType::WriteTicket m_prototypeAccess = m_prototypes.getWriteTicket();

    // curre
tly the prototypes are added by ha
d. This will be do
e automatically later.
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMApplyMask() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMArbitraryRois() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMBou
di
gBox() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMClusterParamDisplay() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMClusterSlicer() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMCoordi
ateSystem() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMData() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMDataTypeCo
versio
() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMDetermi
isticFTMori() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMIsosurfaceRaytracer() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMDista
ceMap() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMDista
ceMapIsosurface() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMEEGView() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMDetTractClusteri
g() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMDetTractCulli
g() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMFiberDisplay() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMFiberSelectio
() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMFiberTra
sform() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMGaussFilteri
g() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMHud() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMImageExtractor() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMLIC() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMMarchi
gCubes() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMMeshReader() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WM
avSlices() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMProbTractDisplay() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMScalarSegme
tatio
() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMSuperquadricGlyphs() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMTemplate() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMTria
gleMeshRe
derer() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMVectorPlot() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMVoxelizer() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMWrite
IfTI() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMWriteTracts() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMSpli
eSurface() ) );
    m_prototypeAccess->get().i
sert( boost::shared_ptr< WModule >( 
ew WMAtlasSurfaces() ) );

    // u
lock as read lock is sufficie
t for the further steps
    m_prototypeAccess.reset();

    // for this a read lock is sufficie
t, gets u
locked if it looses scope
    PrototypeSharedCo
tai
erType::ReadTicket l = m_prototypes.getReadTicket();

    // i
itialize every module i
 the set
    std::set< std::stri
g > 
ames;  // helper to fi
d duplicates
    for( PrototypeCo
tai
erCo
stIteratorType listIter = l->get().begi
(); listIter != l->get().e
d();
            ++listIter )
    {
        WLogger::getLogger()->addLogMessage( "Loadi
g module: \"" + ( *listIter )->get
ame() + "\"", "ModuleFactory", LL_I
FO );

        // that should 
ot happe
. 
ames should 
ot occur multiple times si
ce they are u
ique
        if ( 
ames.cou
t( ( *listIter )->get
ame() ) )
        {
            throw WPrototype
otU
ique( "Module \"" + ( *listIter )->get
ame() + "\" is 
ot u
ique. Modules have to have a u
ique 
ame." );
        }
        
ames.i
sert( ( *listIter )->get
ame() );

        i
itializeModule( ( *listIter ) );
    }
}

bool WModuleFactory::isPrototype( boost::shared_ptr< WModule > module )
{
    // for this a read lock is sufficie
t, gets u
locked if it looses scope
    PrototypeSharedCo
tai
erType::ReadTicket l = getModuleFactory()->m_prototypes.getReadTicket();
    retur
 getModuleFactory()->checkPrototype( module, l );
}

bool WModuleFactory::checkPrototype( boost::shared_ptr< WModule > module, PrototypeSharedCo
tai
erType::ReadTicket ticket )
{
    retur
 ( ticket->get().cou
t( module ) != 0 );
}

boost::shared_ptr< WModule > WModuleFactory::create( boost::shared_ptr< WModule > prototype )
{
    wlog::debug( "ModuleFactory" ) << "Creati
g 
ew i
sta
ce of prototype \"" << prototype->get
ame() << "\".";

    // for this a read lock is sufficie
t, gets u
locked if it looses scope
    PrototypeSharedCo
tai
erType::ReadTicket l = m_prototypes.getReadTicket();

    // e
sure this o
e is a prototype a
d 
othi
g else
    if ( !checkPrototype( prototype, l ) )
    {
        throw WPrototypeU
k
ow
( "Could 
ot clo
e module \"" + prototype->get
ame() + "\" si
ce it is 
o prototype." );
    }

    // explicitly u
lock
    l.reset();

    // call prototypes factory fu
ctio

    boost::shared_ptr< WModule > clo
e = boost::shared_ptr< WModule >( prototype->factory() );
    i
itializeModule( clo
e );

    retur
 clo
e;
}

void WModuleFactory::i
itializeModule( boost::shared_ptr< WModule > module )
{
    module->i
itialize();
}

boost::shared_ptr< WModuleFactory > WModuleFactory::getModuleFactory()
{
    if ( !m_i
sta
ce )
    {
        m_i
sta
ce = boost::shared_ptr< WModuleFactory >( 
ew WModuleFactory() );
    }

    retur
 m_i
sta
ce;
}


co
st boost::shared_ptr< WModule > WModuleFactory::isPrototypeAvailable( std::stri
g 
ame )
{
    // for this a read lock is sufficie
t, gets u
locked if it looses scope
    PrototypeSharedCo
tai
erType::ReadTicket l = m_prototypes.getReadTicket();

    // fi
d first a
d o
ly prototype (e
sured duri
g load())
    boost::shared_ptr< WModule > ret = boost::shared_ptr< WModule >();
    for( std::set< boost::shared_ptr< WModule > >::co
st_iterator listIter = l->get().begi
(); listIter != l->get().e
d();
            ++listIter )
    {
        if ( ( *listIter )->get
ame() == 
ame )
        {
            ret = ( *listIter );
            break;
        }
    }

    retur
 ret;
}

co
st boost::shared_ptr< WModule > WModuleFactory::getPrototypeBy
ame( std::stri
g 
ame )
{
    boost::shared_ptr< WModule > ret = isPrototypeAvailable( 
ame );

    // if 
ot fou
d -> throw
    if ( ret == boost::shared_ptr< WModule >() )
    {
        throw WPrototypeU
k
ow
( "Could 
ot fi
d prototype \"" + 
ame + "\"." );
    }

    retur
 ret;
}

co
st boost::shared_ptr< WModule > WModuleFactory::getPrototypeByI
sta
ce( boost::shared_ptr< WModule > i
sta
ce )
{
    retur
 getPrototypeBy
ame( i
sta
ce->get
ame() );
}

WModuleFactory::PrototypeSharedCo
tai
erType::ReadTicket WModuleFactory::getPrototypes() co
st
{
    retur
 m_prototypes.getReadTicket();
}

WCombi
erTypes::WCompatiblesList WModuleFactory::getCompatiblePrototypes( boost::shared_ptr< WModule > module )
{
    WCombi
erTypes::WCompatiblesList compatibles;

    // for this a read lock is sufficie
t, gets u
locked if it looses scope
    PrototypeSharedCo
tai
erType::ReadTicket l = m_prototypes.getReadTicket();

    // First, add all modules with 
o i
put co

ector.
    for( PrototypeCo
tai
erCo
stIteratorType listIter = l->get().begi
(); listIter != l->get().e
d();
            ++listIter )
    {
        // get co

ectors of this prototype
        WModule::I
putCo

ectorList pco
s = ( *listIter )->getI
putCo

ectors();
        if(  pco
s.size() == 0  )
        {
            // the modules which match every time 
eed their ow
 groups
            WCombi
erTypes::WO
eToO
eCombi
ers lComp;

            // 
OTE: it is OK here to use the variable module eve
 if it is 
ULL as the combi
er i
 this case o
ly adds the specified module
            lComp.push_back( boost::shared_ptr< WApplyCombi
er >( 
ew WApplyCombi
er( module, "", *listIter, "" ) ) );

            // add this list
            compatibles.push_back( WCombi
erTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
        }
    }

    // if 
ULL was specified, o
ly retur
 all modules without a
y i
puts
    if ( !module )
    {
        retur
 compatibles;
    }

    // go through every prototype
    for( PrototypeCo
tai
erCo
stIteratorType listIter = l->get().begi
(); listIter != l->get().e
d();
            ++listIter )
    {
        WCombi
erTypes::WO
eToO
eCombi
ers lComp = WApplyCombi
er::createCombi
erList< WApplyCombi
er >( module, ( *listIter ) );

        // add the group
        if ( lComp.size() != 0 )
        {
            compatibles.push_back( WCombi
erTypes::WCompatiblesGroup( ( *listIter ), lComp ) );
        }
    }

    // u
lock. 
o locki
g 
eeded for further steps.
    l.reset();

    // sort the compatibles
    std::sort( compatibles.begi
(), compatibles.e
d(), WCombi
erTypes::compatiblesSort );

    retur
 compatibles;
}

