# cdupes
Another "fdupes" tool optimized for low-performance machines.
This tool tries to locate existing duplicates of existing files in an efficient way.

# Cached result
A very basic checksum of scanned file are pre-computed and stored for faster search.
The command **cdupes** used to search files implies you ran before the command **updupes**.

# A mlocate based tool
Cdupes use existing **mlocate** file database and **do not perform itself file scanning**.
This means you must perform an **updatedb** _before_ running the **updupes** command.

# Similar projects
Some similar project exists.
The most famous is **fdupes** while the well known for high performance is **rdfind**.
Similar projects I found are :
* duff
* fdupes
* fslint (findup)
* hadori
* hardlink
* jdupes
* rdfind
