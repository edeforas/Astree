# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

from typing import List
import MaterialAir
import MaterialWater
import MaterialVacuum
import MaterialUnknown
import Glass

class GlassManager:
    _instance = None

    def __init__(self):
        self._vGlass = []
        self._vCatalogs = []
        self.inject(MaterialAir())
        self.inject(MaterialVacuum())
        self.inject(MaterialWater())

    def __del__(self):
        for glass in self._vGlass:
            del glass  # TODO check destructor is called

    def create(self, material_name: str):
        for glass in self._vGlass:
            if glass.name() == material_name:
                return glass.clone()

        # if glass does not exist, try to add N- behind (Schott lead free glasses)
        original_material_name = material_name
        if "N-" not in material_name:
            material_name = "N-" + material_name
            for glass in self._vGlass:
                if glass.name() == material_name:
                    return glass.clone()

        # error case
        unknown_material = MaterialUnknown()
        unknown_material.set_formula("unknown")
        unknown_material.set_name(original_material_name)
        unknown_material.set_maker("unknown_glass")
        return unknown_material

    def destroy(self, material: 'Glass'):
        del material

    @classmethod
    def singleton(cls):
        if cls._instance is None:
            cls._instance = GlassManager()  # TODO delete at exit
        assert cls._instance is not None
        return cls._instance

    def list_available(self, available_glass_names: List[str]):
        available_glass_names.clear()
        for glass in self._vGlass:
            available_glass_names.append(glass.name())

    def list_catalogs(self, catalogs: List[str]):
        catalogs.clear()
        for catalog in self._vCatalogs:
            catalogs.append(catalog)

    def exist(self, glass_name: str) -> bool:
        for glass in self._vGlass:
            if glass.name() == glass_name:
                return True
        return False

    def solid_color(self, material_name: str) -> int:
        for glass in self._vGlass:
            if glass.name() == material_name:
                return glass.solid_color()
        return 0xffffff

    def inject(self, glass: 'Glass'):
        self._vGlass.append(glass)

        # add maker in catalog if not existent
        if glass.maker() not in self._vCatalogs:
            self._vCatalogs.append(glass.maker())