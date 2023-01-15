import io
import os
import OpticalDevice

class TextFileReader:
    """Handle utf8 or utf16 encoding"""
    def __init__(self):
        self._is_utf16 = False
        self._file = None
    
    def open(self, file_path):
        self._is_utf16 = self._is_utf16_file(file_path)
        
        if self._is_utf16:
            self._file = open(file_path, 'rb')
            # Read and skip BOM if present
            bom = self._file.read(2)
            return True
        else:
            self._file = open(file_path, 'r', encoding='utf-8')
            return True
    
    def eof(self):
        pos = self._file.tell()
        is_eof = not self._file.read(1)
        self._file.seek(pos)
        return is_eof
    
    def getline(self):
        line = ''
        if self._is_utf16:
            # Simplified UTF-16 handling - this would need proper implementation
            # for real UTF-16 files
            line = self._file.readline().decode('utf-16le', errors='ignore')
        else:
            line = self._file.readline()
        
        if line:
            line = line.strip()
        return line
    
    def _is_utf16_file(self, file_path):
        with open(file_path, 'rb') as f:
            while True:
                chunk = f.read(1024)
                if not chunk:
                    break
                if b'\x00' in chunk:
                    return True
        return False
    
    def close(self):
        if self._file:
            self._file.close()

class DeviceIoZmx:
    @staticmethod
    def import_zmx(file_path):
        tfr = TextFileReader()
        if not tfr.open(file_path):
            return None
        
        od = OpticalDevice.OpticalDevice()
        od.set_relative_convention(True)
        
        must_use_colored_light = False
        dimension_factor = 1.0
        current_surface = 0
        surface_pending = False
        surface_type = ""
        curvature = 0.0
        thickness = 0.0
        diameter = 0.0
        conic = 0.0
        inner_diameter = 0.0
        aperture_diameter = -1.0
        half_fov = 0.0
        comment = ""
        note = f"Imported from ZMX file: {file_path}\nWarning, import may be inaccurate!\n\n"
        glass_catalog = ""
        
        while not tfr.eof():
            line = tfr.getline()
            if not line or len(line) < 4:
                continue
            
            key = line[:4].upper()
            value = line[4:].strip()
            
            if key in ("SURF", "BLNK"):
                if surface_pending:
                    od.insert_surface(current_surface)
                    od.set_comment(current_surface, comment)
                    od.set_type(current_surface, surface_type)
                    od.set(current_surface, "THICK", dimension_factor * thickness)
                    od.set(current_surface, "CURVATURE", curvature / dimension_factor)
                    od.set(current_surface, "DIAMETER", diameter * dimension_factor)
                    od.set(current_surface, "CONIC", conic)
                    od.set(current_surface, "INNER_DIAMETER", inner_diameter * dimension_factor)
                    surface_pending = False
                
                if key == "SURF":
                    try:
                        current_surface = int(value)
                    except ValueError:
                        tfr.close()
                        return None
                    
                    if current_surface == -1:
                        tfr.close()
                        return None
                    
                    surface_pending = True
                    curvature = 0.0
                    diameter = 0.0
                    surface_type = "Air"
                    conic = 0.0
                    comment = ""
                    inner_diameter = 0.0
            
            elif key == "YFLD":
                try:
                    values = [float(v) for v in value.split()]
                    if values:
                        current_fov = max(values)
                        if current_fov > half_fov:
                            half_fov = current_fov
                except ValueError:
                    pass
            
            elif key == "UNIT":
                unit = value.split()[0].upper() if value else ""
                if unit == "MM":
                    dimension_factor = 1.0
                elif unit == "METER":
                    dimension_factor = 1e3
                elif unit == "CM":
                    dimension_factor = 10.0
                elif unit == "IN":
                    dimension_factor = 25.4
            
            elif key == "TYPE":
                if value not in ("STANDARD", "EVENASPH"):
                    tfr.close()
                    return None
            
            elif key == "CURV":
                try:
                    curvature = float(value)
                except ValueError:
                    pass
            
            elif key == "COMM":
                comment = value
            
            elif key == "STOP":
                surface_type = "stop"
            
            elif key == "NOTE":
                note_value = value[value.find(" ") + 1:] if " " in value else value
                note += note_value + "\n"
            
            elif key == "DISZ":
                if value.upper() == "INFINITY":
                    thickness = 0.0
                else:
                    try:
                        thickness = float(value)
                    except ValueError:
                        pass
            
            elif key == "GCAT":
                glass_catalog = value
            
            elif key == "OBSC":
                try:
                    parts = value.split()
                    if len(parts) >= 2:
                        inner_diameter = float(parts[1])
                except ValueError:
                    pass
            
            elif key == "CONI":
                try:
                    conic = float(value)
                except ValueError:
                    pass
            
            elif key == "DIAM":
                try:
                    diameter = float(value) * 2.0
                except ValueError:
                    pass
            
            elif key == "ENPD":
                try:
                    aperture_diameter = float(value)
                except ValueError:
                    pass
            
            elif key == "GLAS":
                first_word = value.split()[0] if value else ""
                if first_word == "MIRROR":
                    surface_type = "reflect"
                else:
                    surface_type = first_word
                    must_use_colored_light = True
        
        tfr.close()
        
        if surface_pending:
            od.insert_surface(current_surface)
            od.set_comment(current_surface, comment)
            od.set_type(current_surface, surface_type)
            od.set(current_surface, "THICK", dimension_factor * thickness)
            od.set(current_surface, "CURVATURE", curvature / dimension_factor)
            od.set(current_surface, "DIAMETER", diameter * dimension_factor)
            od.set(current_surface, "CONIC", conic)
            od.set(current_surface, "INNER_DIAMETER", inner_diameter * dimension_factor)
            surface_pending = False
        
        if note:
            od.set_note(note)
        
        # Set the last surface type as image
        last_surface = od.nb_surface() - 1
        if last_surface >= 0:
            if od.type(last_surface) == "Air":
                od.set_type(last_surface, "image")
                od.set(last_surface, "AUTO_DIAMETER", 1.0)
                od.set_image_autocurvature(True)
            
            if aperture_diameter >= 0:
                od.set(0, "DIAMETER", aperture_diameter * dimension_factor)
        
        # Add colored light in case of glass
        if must_use_colored_light:
            od.set_light_colors("Red.Yellow.Green.Blue.")
        
        # Set field of view
        od.set_half_field_of_view(half_fov)
        
        return od
