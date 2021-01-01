// entry point for customizer script. This probably isn't useful to most people,
// as it's just a wrapper that helps generate customizer.scad for thingiverse.

/* [Basic-Settings] */

// what preset profile do you wish to use? disable if you are going to set paramters below
key_profile = "g20"; // [dcs, oem, dsa, sa, g20, disable]
// what key profile row is this keycap on? 0 for disable
row = 2; // [5,1,2,3,4,0]

// What does the top of your key say?
legend = "1";

/* [Basic-Settings] */

// Length in units of key. A regular key is 1 unit; spacebar is usually 6.25
$key_length = 1.0; // Range not working in thingiverse customizer atm [1:0.25:16]

// What type of stem you want. Most people want Cherry.
$stem_type = "cherry";  // [cherry, alps, rounded_cherry, box_cherry, filled, disable]

// The stem is the hardest part to print, so this variable controls how much 'slop' there is in the stem
$stem_slop = 0.3; // Not working in thingiverse customizer atm [0:0.01:1]

// Set this to true if you're making a spacebar!
$inverted_dish = false;


// Support type. default is "flared" for easy FDM printing; bars are more realistic, and flat could be for artisans
//flared, flat, bars, disaable 
$support_type = "flat"; // [flared, bars, flat, disable]

// Supports for the stem, as it often comes off during printing. Reccommended for most machines
$stem_support_type = "tines"; // [tines, brim, disabled]

/* [Advanced] */

/* Key */
// Height in units of key. should remain 1 for most uses
$key_height = 1.0;
// Keytop thickness, aka how many millimeters between the inside and outside of the top surface of the key
$keytop_thickness = 1;
// Wall thickness, aka the thickness of the sides of the keycap. note this is the total thickness, aka 3 = 1.5mm walls
$wall_thickness = 3;
// Radius of corners of keycap
$corner_radius = 0.10;
// Width of the very bottom of the key
$bottom_key_width = 18.16;
// Height (from the front) of the very bottom of the key
$bottom_key_height = 18.16;
// How much less width there is on the top. eg top_key_width = bottom_key_width - width_difference
$width_difference = 6;
// How much less height there is on the top
$height_difference = 4;
// How deep the key is, before adding a dish
$total_depth = 11.5;
// The tilt of the dish in degrees. divided by key height
$top_tilt = -6;
// How skewed towards the back the top is (0 for center)
$top_skew = 1.7;

/* Stem */

// How far the throw distance of the switch is. determines how far the 'cross' in the cherry switch digs into the stem, and how long the keystem needs to be before supports can start. luckily, alps and cherries have a pretty similar throw. can modify to have stouter keycaps for low profile switches, etc
$stem_throw = 3;

// How much higher the stem is than the bottom of the keycap.
// Inset stem requires support but is more accurate in some profiles
$stem_inset = 0;
// How many degrees to rotate the stems. useful for sideways keycaps, maybe
$stem_rotation = 0;

/* Shape */

// Key shape type, determines the shape of the key. default is 'rounded square'
$key_shape_type = "rounded_square";
// ISO enter needs to be linear extruded NOT from the center. this tells the program how far up 'not from the center' is
$linear_extrude_height_adjustment = 0;
// How many slices will be made, to approximate curves on corners. Leave at 1 if you are not curving corners
// If you're doing fancy bowed keycap sides, this controls how many slices you take
$height_slices = 1;

/* Misc */
// There's a bevel on the cherry stems to aid insertion / guard against first layer squishing making a hard-to-fit stem.
$cherry_bevel = true;

// How tall in mm the stem support is, if there is any. stem support sits around the keystem and helps to secure it while printing.
$stem_support_height = 0.4;
// Font used for text
// Font size used for text
$font_size = 3.75;
//$font="DejaVu Sans Mono:style=Book";
$font="DejaVu Sans Mono:style=Bold";
$font="Menlo :style=Bold";
// Whether or not to render fake keyswitches to check clearances
$clearance_check = false;
// Use linear_extrude instead of hull slices to make the shape of the key
// Should be faster, also required for concave shapes
$linear_extrude_shape = false;
//should the key be rounded? unnecessary for most printers, and very slow
$rounded_key = false;
//minkowski radius. radius of sphere used in minkowski sum for minkowski_key function. 1.75 for G20
$minkowski_radius = .13;

/* Features */

//insert locating bump
$key_bump = false;
//height of the location bump from the top surface of the key
$key_bump_depth = 0.5;
//distance to move the bump from the front edge of the key
$key_bump_edge = 0.4;

/* [Hidden] */

//list of legends to place on a key format: [text, halign, valign, size]
//halign = "left" or "center" or "right"
//valign = "top" or "center" or "bottom"
// Currently does not work with thingiverse customizer, and actually breaks it
$legends = [];

// Enable stabilizers. If you don't want stabilizers use disable; most other keycaps use Cherry stabilizers
$stabilizer_type = "cherry"; // [cherry, rounded_cherry, alps, disable]

// Ternaries are ONLY for customizer. they will NOT work if you're using this in
// OpenSCAD. you should use stabilized(), openSCAD customizer,
// or set $stabilizers directly
// Array of positions of stabilizers
$stabilizers = $key_length >= 6 ? [[-50, 0], [50, 0]] : $key_length >= 2 ? [[-12,0],[12,0]] : [];

// Where the stems are in relation to the center of the keycap, in units. default is one in the center
// Shouldn't work in thingiverse customizer, though it has been...
$stem_positions = [[0,0]];


// key profile definitions


module g20_row(w,h,n) {
  $bottom_key_width = w;
  $bottom_key_height = h;
  $width_difference = 0.5;
  $height_difference = 0.5;
  $top_tilt = 2.5;
  $top_skew = 0; //0.75;
  $dish_type = "disable";
  // something weird is going on with this and legends - can't put it below 1.2 or they won't show
  $dish_depth = 1.2;
  $dish_skew_x = 0;
  $dish_skew_y = 0;
  $minkowski_radius = 1.00; //1.75;
  $key_bump_depth = 0.6;
  $key_bump_edge = 2;
  //also,
  $rounded_key = true;

  if (n == 5) {
    $total_depth = 6 + abs((n-3) * 0.5);
    $top_tilt =  -18.55;
    children();
  } else if (n == 1) {
    $total_depth = 6 + abs((n-3) * 0.5);
    $top_tilt = (n-3) * 7 + 2.5;
    children();
  } else if (n == 2) {
    $total_depth = 6 + abs((n-3) * 0.5);
    $top_tilt = (n-3) * 7 + 2.5;
    children();
  } else if (n == 3) {
    $total_depth = 6 + abs((n-3) * 0.5);
    $top_tilt = 0; //(n-3) * 7 + 2.5;
    children();
  } else if (n == 4) {
    $total_depth = 6 + abs((n-3) * 0.5);
    $top_tilt = (n-3) * 7 + 2.5;
    children();
  } else {
    children();
  }
}

// man, wouldn't it be so cool if functions were first order
module key_profile(key_profile_type, row) {
   g20_row(row) children();
}

module translate_u(x=0, y=0, z=0){
  translate([x * unit, y*unit, z*unit]) children();
}

module brimmed_stem_support(height = 0.4) {
  $stem_support_type = "brim";
  $stem_support_height = height;
  children();
}

module tined_stem_support(height = 0.4) {
  $stem_support_type = "tines";
  $stem_support_height = height;
  children();
}

module rounded() {
  $rounded_key = true;
  children();
}

module inverted() {
  $inverted_dish = true;
  children();
}

module rotated() {
  $stem_rotation = 90;
  children();
}

module stabilized(mm=12, vertical = false, type="cherry") {
  if (vertical) {
    $stabilizer_type = type;
    $stabilizers = [
    [0,  mm],
    [0, -mm]
    ];

    children();
  } else {
    $stabilizer_type = type;
    $stabilizers = [
      [mm,  0],
      [-mm, 0]
    ];

    children();
  }
}

module inset(val=1) {
  $stem_inset = val;
  children();
}

module cherry(slop) {
  $stem_slop = slop ? slop : $stem_slop;
  $stem_type = "cherry";
  children();
}

module box_cherry(slop) {
  $stem_slop = slop ? slop : $stem_slop;
  $stem_type = "box_cherry";
  children();
}

module flared_support() {
  $support_type = "flared";
  children();
}

module bar_support() {
  $support_type = "bars";
  children();
}

module flat_support() {
  $support_type = "flat";
  children();
}

// JFH - necessary
module legend(text, position=[0,0], size=undef) {
    font_size = size == undef ? $font_size : size;
    $legends = [for(L=[$legends, [[text, position, font_size]]], a=L) a];
    children();
}


// actual mm key width and height
function total_key_width(delta = 0) = $bottom_key_width + $unit * ($key_length - 1) - delta;
function total_key_height(delta = 0) = $bottom_key_height + $unit * ($key_height - 1) - delta;

// actual mm key width and height at the top
function top_total_key_width() = $bottom_key_width + ($unit * ($key_length - 1)) - $width_difference;
function top_total_key_height() = $bottom_key_height + ($unit * ($key_height - 1)) - $height_difference;


// rounded square shape with additional sculpting functions to better approximate

// When sculpting sides, how much in should the tops come
$side_sculpting_factor = 4.5;
// When sculpting corners, how much extra radius should be added
$corner_sculpting_factor = 1;
// When doing more side sculpting corners, how much extra radius should be added
$more_side_sculpting_factor = 0.4;


// side sculpting functions
// bows the sides out on stuff like SA and DSA keycaps
function side_sculpting(progress) = (1 - progress) * $side_sculpting_factor;
// makes the rounded corners of the keycap grow larger as they move upwards
function corner_sculpting(progress) = pow(progress, 2) * $corner_sculpting_factor;

module sculpted_square_shape(size, delta, progress) {
  width = size[0];
  height = size[1];

  width_difference = delta[0];
  height_difference = delta[1];
  // makes the sides bow
  extra_side_size =  side_sculpting(progress);
  // makes the rounded corners of the keycap grow larger as they move upwards
  extra_corner_size = corner_sculpting(progress);

  // computed values for this slice
  extra_width_this_slice = (width_difference - extra_side_size) * progress;
  extra_height_this_slice = (height_difference - extra_side_size) * progress;
  extra_corner_radius_this_slice = ($corner_radius + extra_corner_size);

  square_size = [
    width - extra_width_this_slice,
    height - extra_height_this_slice
  ];

  offset(r = extra_corner_radius_this_slice) {
    offset(r = -extra_corner_radius_this_slice) {
      side_rounded_square(square_size, r = $more_side_sculpting_factor * progress);
    }
  }
}

module side_rounded_square(size, r) {
    iw = size.x - 2 * r;
    ih = size.y - 2 * r;
    resolution = 100; 
    sr = r / resolution * 2;
    sh = ih / resolution;
    sw = iw / resolution;
    union() {
      if (sr > 0) {
        translate([-iw/2, 0]) scale([sr, sh]) circle(d = resolution);
        translate([iw/2, 0]) scale([sr, sh]) circle(d = resolution);
        translate([0, -ih/2]) scale([sw, sr]) circle(d = resolution);
        translate([0, ih/2]) scale([sw, sr]) circle(d = resolution);
      }
        square([iw, ih], center=true);
    }
}
module rounded_square_shape(size, delta, progress, center = true) {
    width = size[0];
    height = size[1];

    width_difference = delta[0];
    height_difference = delta[1];

    // computed values for this slice
    extra_width_this_slice = (width_difference) * progress;
    extra_height_this_slice = (height_difference) * progress;
    extra_corner_radius_this_slice = ($corner_radius);

    offset(r=extra_corner_radius_this_slice){
      square(
        [
          width - extra_width_this_slice - extra_corner_radius_this_slice * 2,
          height - extra_height_this_slice - extra_corner_radius_this_slice * 2
        ],
        center=center
      );
    }
}
module square_shape(size, delta, progress){
  square(size - delta * progress, center = true);
}
module oblong_shape(size, delta, progress) {
  // .05 is because of offset. if we set offset to be half the height of the shape, and then subtract height from the shape, the height of the shape will be zero (because the shape would be [width - height, height - height]). that doesn't play well with openSCAD (understandably), so we add this tiny fudge factor to make sure the shape we offset has a positive width
  height = size[1] - delta[1] * progress - .05;

  if (progress < 0.5) {
  } else {
    offset(r=height / 2) {
      square(size - [height, height] - delta * progress, center=true);
    }
  }
}

module key_shape(size, delta, progress = 0) {
  if ($key_shape_type == "iso_enter") {
    ISO_enter_shape(size, delta, progress);
  } else if ($key_shape_type == "sculpted_square") {
    sculpted_square_shape(size, delta, progress);
  } else if ($key_shape_type == "rounded_square") {
    rounded_square_shape(size, delta, progress);
  } else if ($key_shape_type == "square") {
    square_shape(size, delta, progress);
  } else if ($key_shape_type == "oblong") {
    oblong_shape(size, delta, progress);
  } else {
    echo("Warning: unsupported $key_shape_type");
  }
}

// .005 purely for aesthetics, to get rid of that ugly crosshatch
function cherry_cross(slop, extra_vertical = 0) = [
  // horizontal tine
  [4.03 + slop, 1.15 + slop / 3],
  // vertical tine
  [1.25 + slop / 3, 4.23 + extra_vertical + slop / 3 + .005],
];

// actual mm key width and height
function total_key_width(delta = 0) = $bottom_key_width + $unit * ($key_length - 1) - delta;
function total_key_height(delta = 0) = $bottom_key_height + $unit * ($key_height - 1) - delta;

// actual mm key width and height at the top
function top_total_key_width() = $bottom_key_width + ($unit * ($key_length - 1)) - $width_difference;
function top_total_key_height() = $bottom_key_height + ($unit * ($key_height - 1)) - $height_difference;

// I use functions when I need to compute special variables off of other special variables
// functions need to be explicitly included, unlike special variables, which
// just need to have been set before they are used. hence this file


//whole stem, alps or cherry, trimmed to fit
module stem(stem_type, depth, slop){
    cherry_stem(depth, slop);
}


module brim_support(stem_type, stem_support_height, slop) {
  if(stem_type == "alps") {
    linear_extrude(height=stem_support_height) {
      offset(r=1){
        square($alps_stem + [2,2], center=true);
      }
    }
  } else if (stem_type == "cherry") {
    difference() {
      linear_extrude(height = stem_support_height){
        offset(r=1){
          square(outer_cherry_stem(slop) + [2,2], center=true);
        }
      }

      inside_cherry_cross(slop);
    }
  } else if (stem_type == "rounded_cherry") {
    difference() {
      cylinder(d=$rounded_cherry_stem_d * 2, h=stem_support_height);
      inside_cherry_cross(slop);
    }
  } else if (stem_type == "box_cherry") {
    difference() {
      linear_extrude(height = stem_support_height){
        offset(r=1){
          square(outer_box_cherry_stem(slop) + [2,2], center=true);
        }
      }

      inside_cherry_cross(slop);
    }
  }
}

// JFH  JFH  JFH  This is the one!!!!!!!!  JFH  JFH  JFH 
module inside_cherry_cross(slop) {
  // inside cross
  // translation purely for aesthetic purposes, to get rid of that awful lattice
  switchPostxy = (2.950+0.010);  // Size of the key switch head.
  stemKnockout = 0.250;// a cutout on the very bottom to prevent squishing.
    
  translate([0,0,-0.005]) {
    linear_extrude(height = $stem_throw) {
      square(switchPostxy, center=true);

    }
    // Knockout.
    linear_extrude(height = stemKnockout) {
      square(switchPostxy+(2.00*stemKnockout), center=true);

    }
  }

}


// JFH  JFH  JFH  This is the one!!!!!!!!  JFH  JFH  JFH 
module cherry_stem(depth, slop) {
  difference(){
    // outside shape
    linear_extrude(height = depth) {
      offset(r=1){
        square(outer_cherry_stem(slop) - [2,2], center=true);
      }
    }

    inside_cherry_cross(slop);
  }
}

module centered_tines(stem_support_height) {
  translate([0,0,$stem_support_height / 2]) cube([total_key_width($wall_thickness), 1, $stem_support_height], center = true);
  translate([0,0,$stem_support_height / 2]) cube([1, total_key_height($wall_thickness), $stem_support_height], center = true);
}

module tines_support(stem_type, stem_support_height, slop) {

    difference () {
      union() {
        if ($key_length < 2) translate([0,0,$stem_support_height / 2]) cube([total_key_width($wall_thickness), 1, $stem_support_height], center = true);
        translate([2,0,$stem_support_height / 2]) cube([1, total_key_height($wall_thickness), $stem_support_height], center = true);
        translate([-2,0,$stem_support_height / 2]) cube([1, total_key_height($wall_thickness), $stem_support_height], center = true);
      }

      inside_cherry_cross(slop);
    }
}


//whole stem, alps or cherry, trimmed to fit
module stem_support(support_type, stem_type, stem_support_height, slop){
    if (support_type == "brim") {
      brim_support(stem_type, stem_support_height, slop);
    } else if (support_type == "tines") {
      tines_support(stem_type, stem_support_height, slop);
    } else if (support_type == "disable") {
      children();
    } else {
      echo("Warning: unsupported $stem_support_type");
    }
}

// I use functions when I need to compute special variables off of other special variables
// functions need to be explicitly included, unlike special variables, which
// just need to have been set before they are used. hence this file
// cherry stem dimensions
function outer_cherry_stem(slop) = [5.25 - slop * 2, 5.25 - slop * 2];

// box (kailh) switches have a bit less to work with
function outer_box_cherry_stem(slop) = [8,8]; //[6 - slop, 6 - slop];

// .005 purely for aesthetics, to get rid of that ugly crosshatch
function cherry_cross(slop, extra_vertical = 0) = [
  // horizontal tine
  [4.03 + slop, 1.15 + slop / 3],
  // vertical tine
  [1.25 + slop / 3, 4.23 + extra_vertical + slop / 3 + .005],
];

// actual mm key width and height
function total_key_width(delta = 0) = $bottom_key_width + $unit * ($key_length - 1) - delta;
function total_key_height(delta = 0) = $bottom_key_height + $unit * ($key_height - 1) - delta;

// actual mm key width and height at the top
function top_total_key_width() = $bottom_key_width + ($unit * ($key_length - 1)) - $width_difference;
function top_total_key_height() = $bottom_key_height + ($unit * ($key_height - 1)) - $height_difference;

// figures out the scale factor needed to make a 45 degree wall
function scale_for_45(height, starting_size) = (height * 2 + starting_size) / starting_size;

// complicated since we want the different stems to work well
// also kind of messy... oh well
module flared(stem_type, loft, height) {
  translate([0,0,loft]){
    if (stem_type == "rounded_cherry") {
      linear_extrude(height=height, scale = scale_for_45(height, $rounded_cherry_stem_d)){
        circle(d=$rounded_cherry_stem_d);
      }
    } else if (stem_type == "alps") {
      alps_scale = [scale_for_45(height, $alps_stem[0]), scale_for_45(height, $alps_stem[1])];
      linear_extrude(height=height, scale = alps_scale){
        square($alps_stem, center=true);
      }
    } else if (stem_type == "box_cherry") {
      // always render cherry if no stem type. this includes stem_type = false!
      // this avoids a bug where the keycap is rendered filled when not desired
      cherry_scale = [scale_for_45(height, outer_box_cherry_stem($stem_slop)[0]), scale_for_45(height, outer_box_cherry_stem($stem_slop)[1])];
      linear_extrude(height=height, scale = cherry_scale){
        offset(r=1){
          square(outer_box_cherry_stem($stem_slop) - [2,2], center=true);
        }
      }
    } else {
      // always render cherry if no stem type. this includes stem_type = false!
      // this avoids a bug where the keycap is rendered filled when not desired
      cherry_scale = [scale_for_45(height, outer_cherry_stem($stem_slop)[0]), scale_for_45(height, outer_cherry_stem($stem_slop)[1])];
      linear_extrude(height=height, scale = cherry_scale){
        offset(r=1){
          square(outer_cherry_stem($stem_slop) - [2,2], center=true);
        }
      }
    }
  }
}
module flat(stem_type, loft, height) {
  translate([0,0,loft + 500]){
    cube(1000, center=true);
  }
}
module bars(stem_type, loft, height) {
  translate([0,0,loft + height / 2]){
    cube([2, 100, height], center = true);
    cube([100, 2, height], center = true);
  }
}

module supports(type, stem_type, loft, height) {
  if (type == "flared") {
    flared(stem_type, loft, height);
  } else if (type == "flat") {
    flat(stem_type, loft, height);
  } else if (type == "bars") {
    bars(stem_type, loft, height);
  } else if (type == "disable") {
    children();
  } else {
    echo("Warning: unsupported $support_type");
  }
}
module keybump(depth = 0, edge_inset=0.4) {
  radius = 0.5;
  translate([0, -top_total_key_height()/2 + edge_inset, depth]){
        rotate([90,0,90]) cylinder($font_size, radius, radius, true);
        translate([0,0,-radius]) cube([$font_size, radius*2, radius*2], true);
  }
}

/* [Hidden] */
$fs = .1;
$unit = 19.05;
blue = [.2667,.5882,1];
color2 = [.5412, .4784, 1];
purple = [.4078, .3569, .749];
yellow = [1, .6941, .2];
transparent_red = [1,0,0, 0.15];

// key shape including dish. used as the ouside and inside shape in keytop(). allows for itself to be shrunk in depth and width / height
module shape(thickness_difference, depth_difference){
  dished(depth_difference, $inverted_dish) {
    color(blue) shape_hull(thickness_difference, depth_difference, 2);
  }
}

// shape of the key but with soft, rounded edges. much more realistic, MUCH more complex. orders of magnitude more complex
module rounded_shape() {
  color(blue) minkowski(){
    // half minkowski in the z direction
    shape($minkowski_radius * 2, $minkowski_radius/2);
    difference(){
      sphere(r=$minkowski_radius, $fn=20);
      translate([0,0,-$minkowski_radius]){
        cube($minkowski_radius * 2, center=true);
      }
    }
  }
}


// basic key shape, no dish, no inside
// which is only used for dishing to cut the dish off correctly
// $height_difference used for keytop thickness
// extra_slices is a hack to make inverted dishes still work
module shape_hull(thickness_difference, depth_difference, extra_slices = 0){
  render() {
    if ($linear_extrude_shape) {
      linear_extrude_shape_hull(thickness_difference, depth_difference, extra_slices);
    } else {
      hull_shape_hull(thickness_difference, depth_difference, extra_slices);
    }
  }
}

// corollary is hull_shape_hull
// extra_slices unused, only to match argument signatures
module linear_extrude_shape_hull(thickness_difference, depth_difference, extra_slices = 0){
  height = $total_depth - depth_difference;
  width_scale = top_total_key_width() / total_key_width();
  height_scale = top_total_key_height() / total_key_height();

  translate([0,$linear_extrude_height_adjustment,0]){
    linear_extrude(height = height, scale = [width_scale, height_scale]) {
        translate([0,-$linear_extrude_height_adjustment,0]){
        key_shape(total_key_width(thickness_difference), total_key_height(thickness_difference));
      }
    }
  }
}

module hull_shape_hull(thickness_difference, depth_difference, extra_slices = 0) {
  for (index = [0:$height_slices - 1 + extra_slices]) {
    hull() {
      shape_slice(index / $height_slices, thickness_difference, depth_difference);
      shape_slice((index + 1) / $height_slices, thickness_difference, depth_difference);
    }
  }
}

module shape_slice(progress, thickness_difference, depth_difference) {
  skew_this_slice = $top_skew * progress;
  depth_this_slice = ($total_depth - depth_difference) * progress;
  tilt_this_slice = -$top_tilt / $key_height * progress;

  translate([0, skew_this_slice, depth_this_slice]) {
    rotate([tilt_this_slice,0,0]){
      linear_extrude(height = 0.001){
        key_shape(
          [
            total_key_width(thickness_difference),
            total_key_height(thickness_difference)
          ],
          [$width_difference, $height_difference],
          progress
        );
      }
    }
  }
}

// for when you want something to only exist inside the keycap.
// used for the support structure
module inside() {
  intersection() {
    shape($wall_thickness, $keytop_thickness);
    children();
  }
}

// put something at the top of the key, with no adjustments for dishing
module top_placement(depth_difference) {
  translate([$dish_skew_x, $top_skew + $dish_skew_y, $total_depth - depth_difference]){
    rotate([-$top_tilt / $key_height,0,0]){
      children();
    }
  }
}



module envelope(depth_difference) {
  s = 1.5;
  hull(){
    cube([total_key_width() * s, total_key_height() * s, 0.01], center = true);
    top_placement(0.005 + depth_difference){
      cube([top_total_key_width() * s, top_total_key_height() * s, 0.01], center = true);
    }
  }
}

module dished_for_show() {
  difference(){
    union() {
      envelope();
      if ($inverted_dish) top_placement(0) _dish();
    }
    if (!$inverted_dish) top_placement(0) _dish();
  }
}


// for when you want to take the dish out of things
// used for adding the dish to the key shape and making sure stems don't stick out the top
// creates a bounding box 1.5 times larger in width and height than the keycap.
module dished(depth_difference, inverted = false) {
  intersection() {
    children();
    difference(){
      union() {
        envelope(depth_difference);
 //       if (inverted) top_placement(depth_difference) _dish();
      }
 //     if (!inverted) top_placement(depth_difference) _dish();
    }
  }
}

// puts it's children at the center of the dishing on the key, including dish height
// more user-friendly than top_placement
module top_of_key(){
  // if there is a dish, we need to account for how much it digs into the top
  dish_depth = ($dish_type == "disable") ? 0 : $dish_depth;
  // if the dish is inverted, we need to account for that too. in this case we do half, otherwise the children would be floating on top of the dish
  corrected_dish_depth = ($inverted_dish) ? -dish_depth / 2 : dish_depth;

  top_placement(corrected_dish_depth) {
    children();
  }
}

module keytext(text, position, font_size, depth) {
  woffset = (top_total_key_width()/3.5) * position[0];
  hoffset = (top_total_key_height()/3.5) * -position[1];
  translate([woffset, hoffset, -depth]){
    linear_extrude(height=$dish_depth){
      text(text=text, font=$font, size=font_size, halign="center", valign="center");
    }
  }
}

module keystem_positions(positions) {
  for (connector_pos = positions) {
    translate(connector_pos) {
      rotate([0, 0, $stem_rotation]){
        children();
      }
    }
  }
}

module support_for(positions, stem_type) {
  keystem_positions(positions) {
    color(yellow) supports($support_type, stem_type, $stem_throw, $total_depth - $stem_throw);
  }
}

module stems_for(positions, stem_type) {
  keystem_positions(positions) {
    color(yellow) stem(stem_type, $total_depth, $stem_slop);
    if ($stem_support_type != "disable") {
      color(color2) stem_support($stem_support_type, stem_type, $stem_support_height, $stem_slop);
    }
  }
}

// a fake cherry keyswitch, abstracted out to maybe replace with a better one later
module cherry_keyswitch() {
  union() {
    hull() {
      cube([15.6, 15.6, 0.01], center=true);
      translate([0,1,5 - 0.01]) cube([10.5,9.5, 0.01], center=true);
    }
    hull() {
      cube([15.6, 15.6, 0.01], center=true);
      translate([0,0,-5.5]) cube([13.5,13.5,0.01], center=true);
    }
  }
}

//approximate (fully depressed) cherry key to check clearances
module clearance_check() {
  if($stem_type == "cherry" || $stem_type == "cherry_rounded"){
    color(transparent_red){
      translate([0,0,3.6 + $stem_inset - 5]) {
        cherry_keyswitch();
      }
    }
  }
}

// legends / artisan support
module artisan(depth) {
  top_of_key() {
    // outset legend
    if (len($legends) > 0) {
      for (i=[0:len($legends)-1]) {
        keytext($legends[i][0], $legends[i][1], $legends[i][2], depth);
      }
    }
    // artisan objects / outset shape legends
    children();
  }
}

// key with hollowed inside but no stem
module keytop() {
  difference(){
    if ($rounded_key) {
      rounded_shape();
    } else {
      shape(0, 0);
    }
  }
}


//gridx = 19.00; // keypad grid pitch in mm, x axis
//gridy = 19.00; // keypad grid pitch in mm, y axis
//keyx = 16.00;  // width of a key in mm
//keyy = 12.00;  // height of a key in mm

gridx = 15.00; // keypad grid pitch in mm, x axis
gridy = 10.00; // keypad grid pitch in mm, y axis
keyx = 12.50+0.30;  // width of a key in mm
keyy = 7.50+0.30;  // height of a key in mm


module translate_j(x=0, y=0, z=0){

  translate([x * gridx, y*gridy, z]) children();
}


module rpn_calc(profile) {

   for(j = [0:3]) {
     for (i = [0:4]) {
        idx = i+5*j;
        z = 0;
        if(idx == 0)
            translate_j(i,j,z) g20_row(w=keyx,h=keyy, n=3)  cherry() keytop(); // "MOD"
        else
            translate_j(i,j,z) g20_row(w=keyx,h=keyy, n=3)  cherry() keytop(); 
      }
  }
  
  translate_j(0.00, -1.00,0) g20_row(w=keyx,h=keyy,n=3)  cherry() keytop(); // LEFT ARROW
  translate_j(1.00, -1.00,0) g20_row(w=keyx,h=keyy,n=3)  cherry() keytop(); // Zero
  translate_j(2.00, -1.00,0) g20_row(w=keyx,h=keyy,n=3)  cherry() keytop();  // POINT
  translate_j(3.50 ,-1.00,0) g20_row(w=(gridx+keyx),h=keyy,n=3)  cherry() keytop();// "Enter"

 
 for(j = [0:4]) {
    z = 0;
    translate_j(-1,(j-1),z) g20_row(w=keyx,h=keyy,n=3)  cherry() keytop(); // "BASE","DSP","CHS","X-Y","R^"  
    translate_j(-2.5,(j-1)) g20_row(w=(1.50 * keyx),h=keyy,n=3)  cherry() keytop();
 }

 }
 
plateThickness = 3.00;
plateTranslation = 2.50;
standoffHeight = 6.25;
 
cornerx = -60.00;
cornery = -21.00;
module rpn_frame() {
    translate( [10,10, plateTranslation] )  color("green")  cube( [140, 62, plateThickness], center=true );

   for(i = [-1:6]) {
        if(i==-1)
            translate( [cornerx+36.48+0.25+(gridx*(1.75*i)),cornery+0, ((plateThickness/2.00)-standoffHeight-0.25)] )   color("red")   cube( [1.5, 62, standoffHeight], center=false );
        else if(i==5)
            translate( [cornerx+36.48+0.25+(gridx*i),cornery+16, ((plateThickness/2.00)-standoffHeight-0.25)] )   color("red")   cube( [1.5, 46, standoffHeight], center=false );
        else
            translate( [cornerx+36.48+0.25+(gridx*i),cornery+0, ((plateThickness/2.00)-standoffHeight-0.25)] )   color("red")   cube( [1.5, 62, standoffHeight], center=false );           
     }
}

difference (){
    rpn_frame();
    rpn_calc("g20");
};
