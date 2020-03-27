c:
	$(MAKE) -C eomaia
	$(MAKE) -C client
	./client/ftpClient

s:
	$(MAKE) -C eomaia
	$(MAKE) -C server
	./server/a.out

