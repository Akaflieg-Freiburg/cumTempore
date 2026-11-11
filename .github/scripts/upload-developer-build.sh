#!/usr/bin/env bash
# Uploads the given files to the pre-release "developerBuilds", creating it
# if necessary. Needs GH_TOKEN with contents: write.
set -euo pipefail

if ! gh release view developerBuilds > /dev/null 2>&1; then
    gh release create developerBuilds --prerelease \
        --title "Developer builds" \
        --notes "Automatic builds of the develop branch. Not for production use."
fi
gh release upload --clobber developerBuilds "$@"
