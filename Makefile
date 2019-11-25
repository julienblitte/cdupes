CC=gcc
CP=cp
DEST=/opt/bin/
MLCDB=\"/opt/var/mlocate/mlocate.db\"
CDPDB=\"/opt/var/mlocate/cdupes.db\"
RM=rm

.PHONY: ALL install clean

ALL: updupes cdupes test1_mlocate_adler test2_dupesdb

updupes: adler32.c mlocate.c updupes.c cdupes_db.c futils.c
	$(CC) -DMLOCATE_DB=$(MLCDB) -DCDUPES_DB=$(CDPDB) -o $@ $^

cdupes: adler32.c mlocate.c cdupes.c cdupes_db.c futils.c
	$(CC) -DMLOCATE_DB=$(MLCDB) -DCDUPES_DB=$(CDPDB) -o $@ $^

test1_mlocate_adler: adler32.c mlocate.c test1_mlocate_adler.c cdupes_db.c futils.c
	$(CC) -DMLOCATE_DB=$(MLCDB) -DCDUPES_DB=$(CDPDB) -o $@ $^

test2_dupesdb: adler32.c mlocate.c test2_dupesdb.c cdupes_db.c futils.c
	$(CC) -DMLOCATE_DB=$(MLCDB) -DCDUPES_DB=$(CDPDB) -o $@ $^

install: updupes cdupes
	$(CP) $^ $(DEST)

clean:
	$(RM) updupes cdupes test1_mlocate_adler test2_dupesdb
