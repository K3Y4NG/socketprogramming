# Makefile for EE 450
#
#     gmake all
#        Compiles all the files
#
#     gmake healthserver
#        Compiles the health server. creates exe server.
#
#     gmake patient1
#        Compiles patient1. creates exe pat1.
#
#     gmake patient2
#        Compiles patient2. creates exe pat2.
#
#     gmake doc1
#        Compiles doctor1. creates exe doc1.
#
#     gmake doc2
#        Compiles doctor2. creates exe doc2.
#

# need to use gmake


all: healthserver patient1 patient2 doc1 doc2

healthserver: 
	g++ -o server healthcenterserver.cpp -lsocket -lnsl -lresolv

patient1:
	g++ -o pat1 patient1.cpp -lsocket -lnsl -lresolv

patient2:
	g++ -o pat2 patient2.cpp -lsocket -lnsl -lresolv

doc1:
	g++ -o doc1 doctor1.cpp -lsocket -lnsl -lresolv

doc2:
	g++ -o doc2 doctor2.cpp -lsocket -lnsl -lresolv

