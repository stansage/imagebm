### Building

Clone sources
`git clone bitbacket`

Change working directory to source
`cd imagebm`

Initialize submodules
`git submodule init`

Update submodules
`git submodule update`

Make build directory
`mkdir imagebm-build`

Change working directory to build
`cd imagebm-build`

Create VisualStudio solution
`cmake -DCMAKE_BUILD_TYPE=Release ..\imagebm`

Build binaries
`msbuild /t:Build /p:Configuration=Release imagebm.sln`
