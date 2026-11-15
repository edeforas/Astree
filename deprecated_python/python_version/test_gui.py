#!/usr/bin/env python3
"""
Test script for Astree Python GUI
"""

import sys
import os

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(__file__))

def test_gui_imports():
    """Test that GUI imports work correctly."""
    try:
        # Import matplotlib with proper backend
        import matplotlib
        matplotlib.use('Qt5Agg')
        from astree_gui import AstreeMainWindow, PlotCanvas
        print("✓ GUI imports successful")
        return True
    except ImportError as e:
        print(f"✗ GUI import failed: {e}")
        return False

def test_optical_imports():
    """Test that optical simulation imports work."""
    try:
        from optical_device import OpticalDevice
        from device_io import load_device
        print("✓ Optical simulation imports successful")
        return True
    except ImportError as e:
        print(f"✗ Optical simulation import failed: {e}")
        return False

def test_design_loading():
    """Test loading a design file."""
    try:
        from device_io import load_device
        od = load_device("c:/dev/Astree/samples/telescopes/Maksutov.astree")
        if od and od.nb_surface() > 0:
            print(f"✓ Design loading successful: {od.nb_surface()} surfaces")
            return True
        else:
            print("✗ Design loading failed")
            return False
    except Exception as e:
        print(f"✗ Design loading error: {e}")
        return False

def main():
    """Run all tests."""
    print("Astree Python GUI Test Suite")
    print("=" * 40)

    tests = [
        test_gui_imports,
        test_optical_imports,
        test_design_loading,
    ]

    passed = 0
    total = len(tests)

    for test in tests:
        if test():
            passed += 1
        print()

    print(f"Results: {passed}/{total} tests passed")

    if passed == total:
        print("\n🎉 All tests passed! The GUI should work correctly.")
        print("To run the GUI application:")
        print("  python astree_gui.py")
        print("  python astree_gui.py path/to/design.astree  # Load design on startup")
    else:
        print("\n❌ Some tests failed. Please check the errors above.")

if __name__ == "__main__":
    main()