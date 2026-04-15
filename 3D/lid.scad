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

module lid() {
    window_size = 27;
    // Pencere konumu
    win_x = (internal_w + 2*wall - window_size) / 2;
    win_y = (internal_h + 2*wall - window_size) / 2;

    // Ekranın PCB'sinin oturacağı toplam alan (Gömme alanı)
    pocket_w = disp_pcb_w + 0.5;
    pocket_h = disp_pcb_h + 0.5;
    // PCB'nin kapağa göre konumu (Offsetler dahil)
    pocket_x = win_x - 8; 
    pocket_y = win_y - 3;
    
    standoff_base_x = win_x - 8 + (disp_pcb_w - disp_pattern_x)/2;
    standoff_base_y = win_y - 3 + (disp_pcb_h - disp_pattern_y)/2;

    difference() {
        // Ana Kapak Plakası
        rounded_box(internal_w + 2*wall, internal_h + 2*wall, wall, box_radius);
        
        translate([pocket_x, pocket_y, wall - 1.3])
            cube([pocket_w, pocket_h, 1.4]);

        // Ekran Penceresi (Dışarıya açılan delik)
        translate([win_x, win_y, -1])
            cube([window_size, window_size, wall + 2]);
        
        // Havşalı Vida Delikleri (M3)
        for(pos = [[wall + pillar_s/2, wall + pillar_s/2], 
                   [internal_w + wall - pillar_s/2, wall + pillar_s/2], 
                   [internal_w + wall - pillar_s/2, internal_h + wall - pillar_s/2], 
                   [wall + pillar_s/2, internal_h + wall - pillar_s/2]]) {
            translate([pos[0], pos[1], -1]) {
                cylinder(d=3.8, h=wall + 2);
                translate([0,0, 0.9]) 
                    cylinder(d1=7.0, d2=3.8, h=1.5);
            }
        }
        
        // Ekran Sabitleme Vidaları (Dışarıdan atılacak delikler)
        for(x = [0, disp_pattern_x], y = [0, disp_pattern_y]) {
            translate([standoff_base_x + x, standoff_base_y + y, -1]) {
                // Vida geçiş deliği
                cylinder(d=disp_screw_hole, h=wall + 2);
                // Dış yüzeydeki havşa deliği (Vida başı sıfır olması için)
                translate([0, 0, 1.0]) 
                    cylinder(d1=disp_countersink, d2=disp_screw_hole, h=1.5);
            }
        }
    }
    
    // Kılavuz Kenarı
    translate([wall + lip_clearance, wall + lip_clearance, wall])
    difference() {
        // Ana Kılavuz Çerçevesi
        rounded_box(internal_w - 2*lip_clearance, internal_h - 2*lip_clearance, lip_height, box_radius - wall - lip_clearance);
        
        // Çerçevenin içini boşalt (Sadece bir kenar kalsın)
        translate([1.5, 1.5, -1])
            rounded_box(internal_w - 2*lip_clearance - 3, internal_h - 2*lip_clearance - 3, lip_height + 2, box_radius - wall - lip_clearance - 1.5);
            
        // Köşe Sütunları için Boşluklar (Kılavuz kenarı sütunlara çarpmamalı)
        pillar_clearance = 0.5;
        for(pos = [[-1, -1], 
                   [internal_w - pillar_s - 2*lip_clearance - pillar_clearance, -1], 
                   [internal_w - pillar_s - 2*lip_clearance - pillar_clearance, internal_h - pillar_s - 2*lip_clearance - pillar_clearance], 
                   [-1, internal_h - pillar_s - 2*lip_clearance - pillar_clearance]]) {
            translate([pos[0], pos[1], -0.1])
                cube([pillar_s + pillar_clearance + 1, pillar_s + pillar_clearance + 1, lip_height + 1]);
        }
    }

}

lid();