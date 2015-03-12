/////////////////////////////////////////////////////////////////////////////////////////
// 1. Load and position both raw mesh files
/////////////////////////////////////////////////////////////////////////////////////////
 
// load nut and transform to target position
module nut()
{
	translate( [107, 0, 258.25] )
		scale( v = [ 0.95, 1, 1 ] )
			rotate( a = [0, 0, 5] )
				rotate( a = [-92, 0, 0] )
					rotate( a = [0, 0, 45] )
						color( "Peru" ) import( file = "ow_logo_object_nut.stl" );
}

// load brain and transform to target position
module brain()
{
  brainToNutScale = 1.7;
	scale( v = [brainToNutScale, brainToNutScale, brainToNutScale] )
	  scale( v = [ 1.1, 0.96, 1] )
		  rotate( a = [ 0, 0, 0.55 ] )
		    translate( v = [ -6.8, 2.0, 70 ] )
			    mirror( [0, 0, 1] )
					  color( "White" ) import( file = "ow_logo_object_brain.stl" ); 

}

// the nut bounding box (somehow)
module nut_cube()
{
	translate( [0, 0, 0] ) 
		color( "Peru", 0.5 )
			cube( size = [275,330,167], center = false );
}

// the brain bounding box (somehow)
module brain_cube()
{
	translate( [0, -0, -167] ) 
		color( "White", 0.5 )
			cube( size = [275,330,167], center = false ); 
}

module brainnutplaced()
{
  import( file = "both_halfs.stl" );
}


/////////////////////////////////////////////////////////////////////////////////////////
// 2. Create additional parts, like a stand and connectors between them
/////////////////////////////////////////////////////////////////////////////////////////

// create pyramid on xy plane
module pyramid()
{
  // taken from OpenSCAD Wikibook
  polyhedron(
    points=[ [ 1, 1, 0 ], [ 1, -1, 0 ], [ -1, -1, 0 ], [ -1, 1, 0 ], // the four points at base
             [ 0, 0, 1 ]  ],                                 // the apex point 
    triangles=[ [ 0, 1, 4 ], [ 1, 2, 4 ], [ 2, 3, 4 ], [ 3, 0, 4 ],          // each triangle side
                [ 1, 0, 3 ], [ 2, 1, 3 ] ]                                   // two triangles for square base
  );
}

// create a connecting part: brain-nut - simple cube
module connector_nutbrain()
{
  // how deep should the casing go into the object?
  depth = 45;
  wallwidth = 2.5;
  translate( v = [ wallwidth, wallwidth, 0 ] )
    translate( v = [ 123, 148, 0 ] )
      color( "green" ) cube( size = [ 25 - 2 * wallwidth, 25  - 2 * wallwidth, depth ] );
}

// as the nut and brain meshed are not solid, we add a casing for the connection peace to 
module connector_nutbrain_casing()
{
  // how deep should the casing go into the object?
  depth = 50;
  // carve out the connector
  translate( v = [ 123, 148, 0 ] )
    color( "red" ) cube( size = [ 25, 25, depth ] );
}

// create a stand for the Nut-Brain construction
module stand()
{
  union()
  {
    translate( v = [ 0, -20, 0 ] )
      scale( v = [ 1, 1, 0.5 ] ) 
      {
        // drill the casing into the plate
        difference() 
        {
          union()
          {
            connector_nutbrain_casing();
            translate( v = [ 135.5, 160.5, 0 ] )
              color( "red" ) 
                scale( v = [ 25, 25, 200 ] ) pyramid();
          }
          translate( v =[ 105, 135, 70 ] )
            color( "red" ) cube( size = [ 50, 50, 100 ] );
          translate( v = [ 0, 0, -5 ] ) scale( v = [ 1, 1, 10 ] ) connector_nutbrain();
        }
      }

    translate( v = [ 45, 10, 6 ] )
      scale( v = [ 165, 235, 1 ] ) 
        minkowski()
        {
          translate( v = [ 0.05, 0.05, -3 ] ) cylinder( r = 0.1, h = 3, $fn = 25 );
          translate( v = [ 0, 0, -3 ] ) cube( size = [ 1, 1, 3 ] );
        }
 
    translate( v = [ 45, 10, 6 ] )
      scale( v = [ 165, 235, 3 ] ) 
        translate( v = [ 0.2, 1.05, 0 ] ) 
          mirror( [ 0, 1, 0 ] )
            rotate( a = [ 0, 0, 90 ] )
            {
              // Thanks to embossanova:
              // https://cubehero.com/physibles/iamwil/embossanova
              scale( [ 1/1050, 0.15 * 1/125, 1/256 ] )
                surface( file = "logo.dat", convexity = 5 );
            }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// 3. Cut the meshes on the xy plane and place the connector case into the parts
/////////////////////////////////////////////////////////////////////////////////////////

// clean cut at the x-y plane at z = 0 everything  where z < 0
// -> cut the mesh area which will be inside/around the nut
module brain_cut()
{
	difference()
	{
		brain();
		nut_cube();
	}
}

// put a cube inside and carve out a smaller cube to connect brain and nut
module brain_with_connection_caseing()
{
  difference()
  {
    union()
    {
      mirror( v = [ 0, 0, 1 ] ) connector_nutbrain_casing();
      brain_cut();
    }
    mirror( v = [ 0, 0, 1 ] ) translate( v = [ 0, 0, -2 ] ) connector_nutbrain();
  }
}

// clean cut at the x-y plane at z = 0 everything  where z > 0
// -> cut the mesh area which will be inside/around the brain
module nut_cut()
{
	difference()
	{
		nut();
		brain_cube();
	}
}

// put a cube inside and carve out a smaller cube to connect brain and nut
module nut_with_connection_caseing()
{
  difference()
  {
    union()
    {
      // NOTE: carve in the casing through the whole nut. Use lower side for the stand
      scale( v = [ 1, 1, 2.70 ] )  connector_nutbrain_casing();
      nut_cut();
    }
    translate( v = [ 0, 0, -2 ] ) scale( v = [ 1, 1, 3.70 ] ) connector_nutbrain();
  }
}

// create a bunch of connectors here
module sticks( size, num )
{
  for( i = [ 0 : num - 1 ] )
  {
    translate( v = [ 700, ( i * 30 ) - 100, 0 ] ) 
      scale( v = [ size, size, 0.80 * ( 135 / 2 + 45 ) / 45 ] )    // scale in z dir to lengthen the sticks to the half length of the casing through the brain plus the insertion depth (45). Use some additional shrink factor.
        connector_nutbrain();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// 4. Construct the logo and position everything where it belongs for printing
/////////////////////////////////////////////////////////////////////////////////////////

// now build both halfs and translate them to lay on the xy plane with positive z

// HINT: please be aware that the CSG operations with our mesh take up a lot of RAM and
// take a while to calculate. So, for experimenting, please comment out the parts you
// won't need.

// shrink it a bit to match MakerBot's size constraints
scale( 0.25 )
{

  // white parts:
  // mirror to get to positive z
  /*translate( v = [ 280, 0, 0] ) 
    mirror( [0, 0, 1] )
      brain_with_connection_caseing();

  // place stand
  translate( v = [ 550, 20, 0 ] ) 
    scale( v = [ 1.0, 1.0, 1.0 ] ) 
      stand();
*/
  // NOTE: the connectors should fit tight. Please try some scales below to find the best for your printer
  // place connector
  sticks( 0.95, 2 );
  translate( v = [ 0, 60, 0 ] ) sticks( 0.94, 2 );
  translate( v = [ 0, 120, 0 ] ) sticks( 0.93, 2 );

  // blue parts:

  // the nut already is placed where it should be
  //nut_with_connection_caseing();
}


