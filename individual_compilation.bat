@echo off


set namespace="MathInteractives.Interactivities.%@module#%.templates"

echo Begin precompilation


	echo
	echo Processing template %1
	call handlebars "%1.handlebars" -n %namespace% -f "%1.js"
	echo done.

echo Precompilation complete...

goto :eof

:NO_ARGUMENT
echo Invalid input parameters
