# python-raytracer

The goal of this project is to have a small rendering engine capable of working with external files to create and simulate a 3d scene.

## Running
See the Makefile for list of actual build targets. There are no build requirements other than a functioning GCC installation with the standard math library available.

By default the software builds with `-Wall -Wextra -Werror`, so any developer might want to either revise their coding style or turn them off. The `-pipe` option is also turned on to be a bit more SSD friendly.
The output might be a PPM file, which many GNU/Linux native image viewers support.

There are currently no plans to porting to MinGW or Windows in general. But there is nothing in specific to GNU/Linux in the codebase either.


## Configuration

TBD
