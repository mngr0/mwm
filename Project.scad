echo(version=version());

vite=2.5;
diametro_rullo = 10;
buco=diametro_rullo+1;

centroZ=300;
centroY=35-buco+2;
distanzaViti=31;
lato_motore=41;
scavo=lato_motore+2;

engine_rot = 21;
rot_diam=4;

module sensor(){
    linear_extrude(height = 25)
        square([20, 50]);
    
}

module rounded_square( width,height, radius_corner ) {
	translate( [ radius_corner, radius_corner, 0 ] )
		minkowski() {
			square( [width - 2 * radius_corner,height - 2 * radius_corner]  );
			circle( radius_corner ,$fn=30);
		}
}

module engine(){
        linear_extrude(height = lato_motore)
            square([30, lato_motore]);
        translate([30,lato_motore/2,lato_motore/2]){
            rotate([90,0,90])
                cylinder(  engine_rot,   rot_diam, rot_diam,false);
        }
}

module hole(d,x,y,z,h){ 
    translate([x,y,z])
        rotate([0,90,0])
            linear_extrude(height = h)
                circle(d/2,$fn=30);
    }

module guide(){
    linear_extrude(height = 300)
            difference(){
        square([30, 100]);
        translate([0,37])
            square([20,3]);
        translate([0,60])
            square([20,10]);
    }
}
module guide2(){
    linear_extrude(height = 300)
            difference(){
        square([30, 100]);
        translate([0,37])
            square([10,3]);
        translate([0,60])
            square([20,10]);
    }
}
module guide_with_diff(){
    difference(){
        guide2();
    translate([0,27,262.5])
        linear_extrude(height=15.25)
            rounded_square(20,12,2);
    }
}

module baseSide(){
    difference(){
        linear_extrude(height = 30)
            square([200, 100]);
    }
}
module basePlusGuide(){
    baseSide();
    translate([170,0,30])
        guide_with_diff();
    translate([30,00,30])
        mirror([1,0,0])
            guide();
    
}
module topSide(){
    difference(){
        linear_extrude(height = 30)
            square([200, 100]);
        translate([15,60,-1])
            linear_extrude(height = 32)
                square([170, 10]);
    }
}

module maschera(){
        offset=20;
    
        hole(vite,0,centroY+distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,0,centroY+distanzaViti/2,centroZ-distanzaViti/2,100);        
        hole(vite,0,centroY-distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,0,centroY-distanzaViti/2,centroZ-distanzaViti/2,100);
        hole(buco,0,centroY,centroZ,200);
        translate([0,centroY-scavo/2,centroZ+scavo/2+offset])
            rotate([0,90,0])
                linear_extrude(height=22)
                    rounded_square(scavo+offset,scavo,2);
    }

module scanalature_elettronica(){
        //node
        translate([195,50,300])
          rotate([0,90,0])
                linear_extrude(height=10)
                    rounded_square(55,25,2);
        //drv
        translate([195,30,250])
            rotate([0,90,0])
                linear_extrude(height=10)
                    rounded_square(25,45,2);
    }
    
module buchi_cavi(){
    //front hole
    translate([105,10,0])
        rotate([-90,0,0])
            cylinder(  150,   10, 10,false,$fn=30);
    //laterals bottom
    translate([-5,15,0])
        rotate([0,90,0])
            cylinder(  210,   10, 10,false,$fn=30);
    
    //side 1
    translate([10,5,-5])
        linear_extrude(height=205)
            rounded_square(15,15,2);
    //side 2
    translate([175,5,-5])
        linear_extrude(height=240)
            rounded_square(15,15,2);
    //exit 1        
    translate([-20,12.5,200])
        rotate([0,90,0])
            cylinder(  45,   7.5, 7.5,false,$fn=30);        
    //exit 2
    translate([175,12.5,235])
        rotate([0,90,0])
            cylinder(  45,   7.5, 7.5,false,$fn=30);             
    }    
    
module buco_sensore(){

    //scarico connettore
    translate([180,12,30])
        linear_extrude(height=15)
            rounded_square(25,13,2);

    //buco sensore
    translate([160,12,30])
        linear_extrude(height=15)
            rounded_square(25,43,2);
    //retro
    translate([190,12,30])
        linear_extrude(height=15)
            rounded_square(25,43,2);
    hole(3,140,48,36,100);
    hole(3,140,29,36,100);
    
    }    
    
    
module buchi_sensori(){
    buco_sensore();
    
    translate([0,0,100])
        buco_sensore();
    
}


module rullo(){
    translate([25,centroY,centroZ])
            difference(){
            rotate([0,90,0])
                difference(){
                    cylinder(  175,   diametro_rullo/2, diametro_rullo/2,false);
                    cylinder(  20,   (rot_diam+1)/2, (rot_diam+1)/2,false,$fn=30);
                }
                translate([30,0,-10])
                    cylinder(  20,   2, 2,false,$fn=30);
                translate([80,0,-10])
                    cylinder(  20,   2, 2,false,$fn=30);
                translate([130,0,-10])
                    cylinder(  20,   2, 2,false,$fn=30);
            }
    }

module sensore(){
    base_height=1.6;
    base_lenght=33;
    base_width=10;
    sensor_lenght=24;
    sensor_width=7;
    total_height=12;
    sensor_thick=4.5;
    sensor_side=6;
    sensor_center=3;
    sensor_offset=0.5;
    connector_lenght=6;
    connector_height=10;
    hole_diam=3;
    hole_side=1;
    difference(){
        union(){
        linear_extrude(height=base_height)
            square([base_width,base_lenght]);
        translate([(base_width-sensor_width)/2,sensor_offset,base_height])
            linear_extrude(height=base_height)
                square([sensor_width,sensor_lenght]);
        translate([(base_width-sensor_width)/2,sensor_offset+sensor_side,base_height])
            linear_extrude(height=total_height)
                square([sensor_width,sensor_thick]);
        translate([(base_width-sensor_width)/2,sensor_offset+sensor_side+sensor_thick+sensor_center,base_height])
            linear_extrude(height=total_height)
                square([sensor_width,sensor_thick]);
        translate([0,base_lenght-connector_lenght,-connector_height])
            linear_extrude(height=connector_height)
                square([base_width,connector_lenght]); 
        }
        translate([base_width/2,sensor_offset+hole_side+hole_diam/2,0])
            cylinder(10,hole_diam/2,hole_diam/2,false,$fn=30);
        translate([base_width/2,sensor_lenght+sensor_offset-(hole_side+hole_diam/2),0])
            cylinder(10,hole_diam/2,hole_diam/2,false,$fn=30);
    }
}

module total(){
    difference(){
       basePlusGuide();
       maschera();
       scanalature_elettronica();
       buchi_cavi();
    //}
       buchi_sensori();
    }
    //rullo();
    //translate([-8,centroY-lato_motore/2,centroZ-lato_motore/2])
    //engine();
    //translate([184,51,41])
    //rotate([180,90,0])
    //sensore();
}


//minkowski(){
total();
//cylinder(r=2,h=1);
//}
