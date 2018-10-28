echo(version=version());


vite=2.5/2;

diametro_rullo = 7;
buco=diametro_rullo+1;

centroZ=300;
centroY=35-buco+2;
distanzaViti=31;

lato_motore=41;
scavo=lato_motore+2;


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
                cylinder(  20,   5, 5,false);
        }
}

module hole(d,y,z,h){ 
    translate([0,y,z])
        rotate([0,90,0])
            linear_extrude(height = h)
                circle(d,$fn=30);
    }

module guide(){
    linear_extrude(height = 300)
            difference(){
        square([30, 100]);
        translate([0,30])
            square([20,10]);
        translate([0,60])
            square([20,10]);
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
        guide();
    translate([30,100,30])
        rotate([0,0,180])
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
    
        hole(vite,centroY+distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,centroY+distanzaViti/2,centroZ-distanzaViti/2,100);        
        hole(vite,centroY-distanzaViti/2,centroZ+distanzaViti/2,100);
        hole(vite,centroY-distanzaViti/2,centroZ-distanzaViti/2,100);
        hole(buco,centroY,centroZ,200);
        translate([0,centroY-scavo/2,centroZ+scavo/2+offset])
            rotate([0,90,0])
                linear_extrude(height=22)
                    rounded_square(scavo+offset,scavo,2);
    }

module rullo(){
    translate([29,centroY,centroZ])
            rotate([0,90,0])
                cylinder(  171,   diametro_rullo, diametro_rullo,false);
    }

module total(){
    difference(){
        basePlusGuide();
        maschera();
    }
    
    rullo();
    translate([-8,centroY-lato_motore/2,centroZ-lato_motore/2])
        engine();
}



total();

