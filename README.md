# SHAZAM REPLICA

Written on OS X 10.6.4

Assumes macports, with installed ports:

*   libmad
*   sqlite (?) somewhere to store lots of hashing data.
*   freeimage (?) cause i wanna draw spectrums.

## GOAL

Build a duplicate detection system for your library that doesn't suck like itunes.

As for code style... STFU and hack it. A weird combination of C and C++. 
Hey maybe i'll throw some templates in there just to confuse stuff.
One day will be refactoring day, but that day is still far off.

## RESOURCES

The origins of this project is inspired by two blog posts:

* http://laplacian.wordpress.com/2009/01/10/how-shazam-works/
* http://www.redcode.nl/blog/2010/06/creating-shazam-in-java/

The blog posts were inspired by the original paper:

* "An Industrial-Strength Audio Search Algorithm", A. Wang, Shazam Entertainment

Using MAD:

* http://www.underbit.com/products/mad/
* http://www.mars.org/pipermail/mad-dev/2001-October/000369.html

Useful GIT stuff:

* http://gitref.org/
* http://stackoverflow.com/questions/1184518/getting-existing-git-branches-to-track-remote-branches