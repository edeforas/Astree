# This file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# Please see LICENSE.txt for more details and licensing issues
# Copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com
import Properties
import OpticalDevice

class DeviceIo:
    @staticmethod
    def load(s_file):
        prop = Properties()
        if not prop.load(s_file):
            return None

        p_od = OpticalDevice()
        if prop.exist("device.convention"):
            p_od.set_relative_convention(prop.get("device.convention") == "relative")
        else:
            p_od.set_relative_convention(False)

        # new way
        if prop.exist("device.relative_convention"):
            p_od.set_relative_convention(prop.get_bool("device.relative_convention"))

        i_s = 0
        s_surf_name = "0"
        while True:
            s_type = ""
            if prop.exist(f"{s_surf_name}.type"):
                s_type = prop.get(f"{s_surf_name}.type")

            if not s_type:
                break

            if s_type == "observe":
                s_type = "image"  # conversion from old format

            p_od.insert_surface(i_s)
            p_od.set_type(i_s, s_type)

            if prop.exist(f"{s_surf_name}.diameter"):
                d_diameter = prop.get_double(f"{s_surf_name}.diameter")
                p_od.set(i_s, "DIAMETER", d_diameter)

                d_gainclone = 1
                if prop.exist(f"{s_surf_name}.diameter.clone.gain"):
                    d_gainclone = prop.get_int(f"{s_surf_name}.diameter.clone.gain")
                if prop.exist(f"{s_surf_name}.diameter.clone"):
                    p_od.set_clone(i_s, "DIAMETER", prop.get_int(f"{s_surf_name}.diameter.clone"), d_gainclone)

            if prop.exist(f"{s_surf_name}.diameter.auto"):
                p_od.set(i_s, "AUTO_DIAMETER", prop.get_bool(f"{s_surf_name}.diameter.auto"))

            if prop.exist(f"{s_surf_name}.inner_diameter"):
                d_inner_diameter = prop.get_double(f"{s_surf_name}.inner_diameter")
                p_od.set(i_s, "INNER_DIAMETER", d_inner_diameter)

                d_gainclone = 1
                if prop.exist(f"{s_surf_name}.inner_diameter.clone.gain"):
                    d_gainclone = prop.get_int(f"{s_surf_name}.inner_diameter.clone.gain")
                if prop.exist(f"{s_surf_name}.inner_diameter.clone"):
                    p_od.set_clone(i_s, "INNER_DIAMETER", prop.get_int(f"{s_surf_name}.inner_diameter.clone"), d_gainclone)

            if prop.exist(f"{s_surf_name}.inner_diameter.auto"):
                p_od.set(i_s, "AUTO_INNER_DIAMETER", prop.get_bool(f"{s_surf_name}.inner_diameter.auto"))

            if prop.exist(f"{s_surf_name}.radius_curvature"):
                if prop.get(f"{s_surf_name}.radius_curvature") == "inf":
                    p_od.set(i_s, "RADIUS_CURVATURE", "RADIUS_CURVATURE_INFINITY")
                else:
                    p_od.set(i_s, "RADIUS_CURVATURE", prop.get_double(f"{s_surf_name}.radius_curvature"))

                d_gainclone = 1.
                if prop.exist(f"{s_surf_name}.radius_curvature.clone.gain"):
                    d_gainclone = prop.get_int(f"{s_surf_name}.radius_curvature.clone.gain")
                if prop.exist(f"{s_surf_name}.radius_curvature.clone"):
                    p_od.set_clone(i_s, "RADIUS_CURVATURE", prop.get_int(f"{s_surf_name}.radius_curvature.clone"), d_gainclone)

            if prop.exist(f"{s_surf_name}.conic"):
                p_od.set(i_s, "CONIC", prop.get_double(f"{s_surf_name}.conic"))
                d_gainclone = 1
                if prop.exist(f"{s_surf_name}.conic.clone.gain"):
                    d_gainclone = prop.get_int(f"{s_surf_name}.conic.clone.gain")
                if prop.exist(f"{s_surf_name}.conic.clone"):
                    p_od.set_clone(i_s, "CONIC", prop.get_int(f"{s_surf_name}.conic.clone"), d_gainclone)

            if prop.exist(f"{s_surf_name}.r4"):
                p_od.set(i_s, "R4", prop.get_double(f"{s_surf_name}.r4"))

            if prop.exist(f"{s_surf_name}.r6"):
                p_od.set(i_s, "R6", prop.get_double(f"{s_surf_name}.r6"))

            if prop.exist(f"{s_surf_name}.r8"):
                p_od.set(i_s, "R8", prop.get_double(f"{s_surf_name}.r8"))

            if prop.exist(f"{s_surf_name}.r10"):
                p_od.set(i_s, "R10", prop.get_double(f"{s_surf_name}.r10"))

            if prop.exist(f"{s_surf_name}.comment"):
                p_od.set_comment(i_s, prop.get(f"{s_surf_name}.comment"))

            if prop.exist(f"{s_surf_name}.z"):
                d_z = prop.get_double(f"{s_surf_name}.z")
                p_od.set(i_s, "Z", d_z)
                d_gainclone = 1
                if prop.exist(f"{s_surf_name}.z.clone.gain"):
                    d_gainclone = prop.get_int(f"{s_surf_name}.z.clone.gain")
                if prop.exist(f"{s_surf_name}.z.clone"):
                    p_od.set_clone(i_s, "Z", prop.get_int(f"{s_surf_name}.z.clone"), d_gainclone)

            if prop.exist(f"{s_surf_name}.thick"):
                d_thick = prop.get_double(f"{s_surf_name}.thick")
                p_od.set(i_s, "THICK", d_thick)
                d_gainclone = 1
                if prop.exist(f"{s_surf_name}.thick.clone.gain"):
                    d_gainclone = prop.get_int(f"{s_surf_name}.thick.clone.gain")
                if prop.exist(f"{s_surf_name}.thick.clone"):
                    p_od.set_clone(i_s, "THICK", prop.get_int(f"{s_surf_name}.thick.clone"), d_gainclone)

            # old format
            if prop.exist(f"{s_surf_name}.z.autofocus"):
                p_od.set_autofocus(prop.get_bool(f"{s_surf_name}.z.autofocus"))

            # new_format
            if prop.exist("image.autofocus"):
                p_od.set_autofocus(prop.get_bool("image.autofocus"))

            if prop.exist("image.autocurvature"):
                p_od.set_image_autocurvature(prop.get_bool("image.autocurvature"))

            i_s += 1
            s_surf_name = str(i_s)

            # Check for both old and new format conditions to continue
            if not (prop.exist(f"{s_surf_name}.kind") or prop.exist(f"{s_surf_name}.type")):
                break

        # old format
        if prop.exist("half_field_of_view"):
            p_od.set_half_field_of_view(prop.get_double("half_field_of_view"))

        # new format
        if prop.exist("light.half_field_of_view"):
            p_od.set_half_field_of_view(prop.get_double("light.half_field_of_view"))

        if prop.exist("light.nbsteps"):
            p_od.set_nb_intermediate_angles(prop.get_int("light.nbsteps"))
        else:
            p_od.set_nb_intermediate_angles(3)

        if prop.exist("light.colors"):
            p_od.set_light_colors(prop.get("light.colors"))
        else:
            p_od.set_light_colors("")  # temporary

        # old format - read comment
        if prop.exist("note"):
            s_note = prop.get("note")
            p_od.set_note(s_note)

        # new format
        if prop.exist("device.note"):
            s_note = prop.get("device.note")
            p_od.set_note(s_note)

        # load other parameters
        other_params = prop.all()
        for s_key, value in other_params.items():
            if len(s_key) > 10:
                s_sub_key = s_key[:10]
                if s_sub_key == "parameter.":
                    p_od.set_parameter(s_key[10:], value)

        return p_od


    def save(sFile: str, pOD: 'OpticalDevice') -> bool:
        prop = Properties()
        
        for iS in range(pOD.nb_surface()):
            sSurfName = str(iS)
            
            prop.set(f"{sSurfName}.type", pOD.type(iS))
            
            if pOD.relative_convention():
                dThick = pOD.get(iS, "THICK")
                prop.set(f"{sSurfName}.thick", dThick)
                
                clone_result = pOD.get_clone(iS, "THICK")
                if clone_result is not None:
                    iSurfclone, dGainclone = clone_result
                    prop.set(f"{sSurfName}.thick.clone", iSurfclone)
                    prop.set(f"{sSurfName}.thick.clone.gain", dGainclone)
            else:
                dZ = pOD.get(iS, "Z")
                prop.set(f"{sSurfName}.z", dZ)
                
                clone_result = pOD.get_clone(iS, "Z")
                if clone_result is not None:
                    iSurfclone, dGainclone = clone_result
                    prop.set(f"{sSurfName}.z.clone", iSurfclone)
                    prop.set(f"{sSurfName}.z.clone.gain", dGainclone)
            
            if pOD.get_autofocus() and (iS == pOD.nb_surface() - 1):
                prop.set(f"{sSurfName}.z.autofocus", True)
            
            # save diameter
            dDiameter = pOD.get(iS, "DIAMETER")
            prop.set(f"{sSurfName}.diameter", dDiameter)
            if pOD.get(iS, "AUTO_DIAMETER") != 0.:
                prop.set(f"{sSurfName}.diameter.auto", True)
            
            clone_result = pOD.get_clone(iS, "DIAMETER")
            if clone_result is not None:
                iSurfclone, dGainclone = clone_result
                prop.set(f"{sSurfName}.diameter.clone", iSurfclone)
                prop.set(f"{sSurfName}.diameter.clone.gain", dGainclone)
            
            # save inner diameter
            dInnerDiameter = pOD.get(iS, "INNER_DIAMETER")
            if dInnerDiameter != 0.:
                prop.set(f"{sSurfName}.inner_diameter", dInnerDiameter)
                if pOD.get(iS, "AUTO_INNER_DIAMETER") != 0.:
                    prop.set(f"{sSurfName}.inner_diameter.auto", True)
            
            clone_result = pOD.get_clone(iS, "INNER_DIAMETER")
            if clone_result is not None:
                iSurfclone, dGainclone = clone_result
                prop.set(f"{sSurfName}.inner_diameter.clone", iSurfclone)
                prop.set(f"{sSurfName}.inner_diameter.clone.gain", dGainclone)
            
            # save radius curvature
            dRC = pOD.get(iS, "RADIUS_CURVATURE")
            if dRC >= "RADIUS_CURVATURE_INFINITY" / 2:
                prop.set(f"{sSurfName}.radius_curvature", "inf")
            else:
                prop.set(f"{sSurfName}.radius_curvature", dRC)
            
            clone_result = pOD.get_clone(iS, "RADIUS_CURVATURE")
            if clone_result is not None:
                iSurfclone, dGainclone = clone_result
                prop.set(f"{sSurfName}.radius_curvature.clone", iSurfclone)
                prop.set(f"{sSurfName}.radius_curvature.clone.gain", dGainclone)
            
            # save conic
            dConic = pOD.get(iS, "CONIC")
            if dConic != 0.:
                prop.set(f"{sSurfName}.conic", dConic)
            
            clone_result = pOD.get_clone(iS, "CONIC")
            if clone_result is not None:  # TODO dgainclone != 1
                iSurfclone, dGainclone = clone_result
                prop.set(f"{sSurfName}.conic.clone", iSurfclone)
            
            # save higher order terms
            for term in ['R4', 'R6', 'R8', 'R10']:
                value = pOD.get(iS, term)
                if value != 0.:
                    prop.set(f"{sSurfName}.{term.lower()}", value)
            
            # save comment if exists
            comment = pOD.comment(iS)
            if comment:
                prop.set(f"{sSurfName}.comment", comment)
        
        # save image properties
        prop.set("image.autofocus", pOD.get_autofocus())
        prop.set("image.autocurvature", pOD.get_image_autocurvature())
        
        # save light properties
        prop.set("light.half_field_of_view", pOD.half_field_of_view())
        prop.set("light.colors", pOD.light_colors())
        prop.set("light.nbsteps", pOD.nb_intermediate_angles())
        
        # save notes
        sNote = pOD.note()
        if sNote:
            prop.set("note", sNote)
            prop.set("device.note", sNote)
        
        # save convention
        prop.set("device.relative_convention", pOD.relative_convention())
        
        # save other parameters
        otherParams = pOD.all_parameters()
        for key, value in otherParams.items():
            if value:
                prop.set(f"parameter.{key}", value)
        
        return prop.save(sFile)
