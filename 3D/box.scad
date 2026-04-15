// --- PARAMETRELER ---
$fn = 60;
wall = 2.0;
internal_w = 82;
internal_h = 65;
internal_d = 35;
clearance = 0.5;
box_radius = 4.0;

// Kılavuz Kenarı (Lip) Parametreleri
lip_height = 1.5;     // Kapağın içine doğru giren çıkıntı
lip_clearance = 0.3;  // Rahat geçmesi için bırakılan tolerans

// Köşe Sütunları
pillar_s = 8;
insert_dia = 4.2;
insert_depth = 8; 

// Ekran Montajı (Waveshare 1.54")
disp_pcb_w = 48;
disp_pcb_h = 33;
disp_pattern_x = 43;
disp_pattern_y = 28;

// Delik Çapları
disp_screw_hole = 0.0; // M3 vidanın geçmesi için
disp_countersink = 5.0; // M3 vida başı için

// --- YARDIMCI MODÜL: Yuvarlatılmış Dikdörtgen ---
module rounded_box(w, h, d, r) {
    hull() {
        translate([r, r, 0]) cylinder(r=r, h=d);
        translate([w-r, r, 0]) cylinder(r=r, h=d);
        translate([r, h-r, 0]) cylinder(r=r, h=d);
        translate([w-r, h-r, 0]) cylinder(r=r, h=d);
    }
}

module body() {
    difference() {
        // Dış Kabuk (Yuvarlatılmış)
        rounded_box(internal_w + 2*wall, internal_h + 2*wall, internal_d + wall, box_radius);
        
        translate([wall, wall, wall])
            rounded_box(internal_w, internal_h, internal_d + 1, box_radius - wall);
        
        // USB-C Kesimi
        translate([-1, 39 + wall, wall])
            cube([wall + 2, 10, 5]);
    }

    for(pos = [[wall, wall], 
               [internal_w + wall - pillar_s, wall], 
               [internal_w + wall - pillar_s, internal_h + wall - pillar_s], 
               [wall, internal_h + wall - pillar_s]]) {
        translate([pos[0], pos[1], wall])
        difference() {
            cube([pillar_s, pillar_s, internal_d]);
            translate([pillar_s/2, pillar_s/2, internal_d - insert_depth])
                cylinder(d=insert_dia, h=insert_depth + 1);
        }
    }
}

body();