all : ssu_sdup fmd5 fsha1 help
ssu_sdup : ssu_sdup.c
		gcc -o ssu_sdup ssu_sdup.c

fmd5 : ssu_find-md5.c
		gcc -o fmd5 ssu_find-md5.c -lcrypto

fsha1 : ssu_find-sha1.c
		gcc -o fsha1 ssu_find-sha1.c -lcrypto

help : ssu_help.c
		gcc -o help ssu_help.c

clean :
		rm ssu_sdup
		rm ssu_help
		rm find-md5
		rm find-sha1
