#!/usr/bin/env python3
"""
OROGENY: Gold Master Build Verification
Day 14 — Sanity check on the packaged Shipping build.

USAGE:
    python Verify_Build.py <ArchiveDirectory>

CHECKS:
    1. Orogeny.exe exists in the archive
    2. Content/Paks/ contains a .pak file > 50MB (assets were cooked)

EXIT CODES:
    0 = All checks passed
    1 = One or more checks failed
"""

import os
import sys
import glob


def verify_build(archive_dir: str) -> bool:
    """Verify the Gold Master build output."""
    passed = True

    # -----------------------------------------------------------------------
    # Locate the WindowsClient subdirectory
    # -----------------------------------------------------------------------
    # RunUAT archives to: <ArchiveDir>/WindowsClient/
    client_dir = os.path.join(archive_dir, "WindowsClient")
    if not os.path.isdir(client_dir):
        # Try without subdirectory (in case archive layout differs)
        client_dir = archive_dir

    print(f"Verifying build at: {client_dir}")
    print("=" * 60)

    # -----------------------------------------------------------------------
    # Check 1: Orogeny.exe exists
    # -----------------------------------------------------------------------
    exe_path = os.path.join(client_dir, "Orogeny.exe")
    if os.path.isfile(exe_path):
        exe_size_mb = os.path.getsize(exe_path) / (1024 * 1024)
        print(f"  [PASS] Orogeny.exe found ({exe_size_mb:.1f} MB)")
    else:
        # Search recursively for the exe
        found = glob.glob(os.path.join(client_dir, "**", "Orogeny.exe"), recursive=True)
        if found:
            exe_size_mb = os.path.getsize(found[0]) / (1024 * 1024)
            print(f"  [PASS] Orogeny.exe found at {found[0]} ({exe_size_mb:.1f} MB)")
        else:
            print(f"  [FAIL] Orogeny.exe NOT FOUND in {client_dir}")
            passed = False

    # -----------------------------------------------------------------------
    # Check 2: .pak file exists and is > 50MB (assets were cooked)
    # -----------------------------------------------------------------------
    pak_pattern = os.path.join(client_dir, "**", "*.pak")
    pak_files = glob.glob(pak_pattern, recursive=True)

    if pak_files:
        largest_pak = max(pak_files, key=os.path.getsize)
        pak_size_mb = os.path.getsize(largest_pak) / (1024 * 1024)

        if pak_size_mb > 50.0:
            print(f"  [PASS] Pak file found: {os.path.basename(largest_pak)} ({pak_size_mb:.1f} MB)")
        else:
            print(f"  [FAIL] Pak file too small: {pak_size_mb:.1f} MB (expected > 50 MB)")
            print(f"         This suggests assets were not properly cooked.")
            passed = False
    else:
        print(f"  [FAIL] No .pak files found in {client_dir}")
        print(f"         Content was not cooked or staged.")
        passed = False

    # -----------------------------------------------------------------------
    # Summary
    # -----------------------------------------------------------------------
    print("=" * 60)
    if passed:
        print("  RESULT: ALL CHECKS PASSED — Gold Master verified.")
    else:
        print("  RESULT: VERIFICATION FAILED — See errors above.")

    return passed


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <ArchiveDirectory>")
        print(f"Example: {sys.argv[0]} Build/GoldMaster")
        sys.exit(1)

    archive_directory = sys.argv[1]

    if not os.path.isdir(archive_directory):
        print(f"ERROR: Archive directory does not exist: {archive_directory}")
        sys.exit(1)

    success = verify_build(archive_directory)
    sys.exit(0 if success else 1)
