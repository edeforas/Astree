# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

import os
import glob
import platform

class FileUtil:
    @staticmethod
    def get_executable_path():
        if platform.system() == "Windows":
            import ctypes
            executable_path_buffer = ctypes.create_string_buffer(260)
            n_chars = ctypes.windll.kernel32.GetModuleFileNameA(None, executable_path_buffer, 260)
            return executable_path_buffer.value.decode('utf-8')[:n_chars]
        else:  # Assuming Unix-like systems
            return os.readlink("/proc/self/exe")

    @staticmethod
    def list(path_and_mask):        
        return glob.glob(path_and_mask)

    @staticmethod
    def get_path(file_name):
        position_of_last_slash = file_name.rfind("/")
        return file_name[:position_of_last_slash]