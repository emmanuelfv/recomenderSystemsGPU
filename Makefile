#Reglas implícitas-----------------------------------------------------------
#Modulos
lec = lecturaRS
nor = normalizacion
psql = lecturaPSQL

#métodos
ik = itemKNN
ikg = itemKNN_GPU
sik = itemKNN_SP
sikg = itemKNN_SP_GPU
dm = dynamic_index
dmg = dynamic_index_GPU
#pop = POP

#Rutas
bib = bibliotecas/
r1 = implementacion1/
r2 = implementacion2/
r3 = implementacion3/
r4 = pop/

#resultadoFinal
valid = validacion

#El orden en que se desecciona un directorio de búsqueda (con -L) importa,
#puede estar presente -L varias veces. 

##validación-----------------------------------------------------------------
#
val: $(valid).cu $(r1)$(ikg).cu $(bib)$(lec).c $(bib)$(psql).c $(bib)$(nor).c
	cp $(bib)$(lec).c $(bib)$(lec).cu
	cp $(bib)$(nor).c $(bib)$(nor).cu
	cp $(r1)$(ik).c $(r1)$(ik).cu
	cp $(r2)$(sik).c $(r2)$(sik).cu
	cp $(r4)POP.c $(r4)POP.cu

	nvcc --gpu-architecture=sm_75 --device-c $(valid).cu $(r1)$(ikg).cu $(bib)$(lec).cu $(bib)$(nor).cu $(r1)$(ik).cu $(r2)$(sik).cu $(r2)$(sikg).cu $(r4)POP.cu 
	nvcc --gpu-architecture=sm_75 -o Validacion.o $(valid).o $(ikg).o $(lec).o $(nor).o $(ik).o $(sik).o $(sikg).o POP.o -lpq -lgsl -lgslcblas
	rm $(bib)*.cu
	rm $(r1)$(ik).cu
	rm $(r2)$(sik).cu
	rm $(r4)POP.cu


valPrints: $(valid).cu $(r1)$(ikg).cu $(bib)$(lec).c $(bib)$(psql).c $(bib)$(nor).c
	cp $(bib)$(lec).c $(bib)$(lec).cu
	cp $(bib)$(nor).c $(bib)$(nor).cu
	cp $(r1)$(ik).c $(r1)$(ik).cu
	cp $(r2)$(sik).c $(r2)$(sik).cu
	nvcc --gpu-architecture=sm_75 --device-c -DALLOW_PRINTS $(valid).cu $(r1)$(ikg).cu $(bib)$(lec).cu $(bib)$(nor).cu $(r1)$(ik).cu $(r2)$(sik).cu $(r2)$(sikg).cu
	nvcc --gpu-architecture=sm_75 -o Validacion.o $(valid).o $(ikg).o $(lec).o $(nor).o $(ik).o $(sik).o $(sikg).o -lpq -lgsl -lgslcblas
	rm $(bib)*.cu
	rm $(r1)$(ik).cu
	rm $(r2)$(sik).cu

# cp $(bib)$(psql).c $(bib)$(psql).cu
# $(bib)$(psql).cu 
# $(psql).o

##ItemKNN---------------------------------------------------------------------
#No paralelizado
ik1: $(r1)$(ik)_V1.c basicRS
	gcc $(r1)$(ik)_V1.c -o $(ik)_V1.o -lm -lpq -L. -lBasicRS

ik2: $(r1)$(ik)_V2.c basicRS
	gcc $(r1)$(ik)_V2.c -o $(ik)_V2.o -lm -lpq -L. -lBasicRS

ik3: $(r1)$(ik)_V3.c basicRS
	gcc $(r1)$(ik)_V3.c -o $(ik)_V3.o -lm -lpq -L. -lBasicRS

#Versión GPU
ikg: $(r1)$(ikg).cu $(bib)$(lec).c $(bib)$(psql).c $(bib)$(nor).c
	cp $(bib)$(lec).c $(bib)$(lec).cu
	cp $(bib)$(psql).c $(bib)$(psql).cu
	cp $(bib)$(nor).c $(bib)$(nor).cu
	nvcc --gpu-architecture=sm_75 --device-c $(r1)$(ikg).cu $(bib)$(lec).cu $(bib)$(psql).cu $(bib)$(nor).cu
	nvcc --gpu-architecture=sm_75 -o ItemKNN_GPU.o $(ikg).o $(lec).o $(psql).o $(nor).o -lpq
	rm $(bib)*.cu

##ItemKNN_SP------------------------------------------------------------------
iksp: $(r2)$(sik).c basicRS
	gcc $(r2)$(sik).c -o $(sik).o -lm -lpq -L. -lBasicRS


##Dynamic_Index---------------------------------------------------------------


#Bibliotecas-(no paralelo)----------------------------------------------------
#biblioteca
basicRS: lect psql norm
	ar rcs libBasicRS.a lect.o norm.o psql.o

#modulos
lect :  $(bib)$(lec).c $(bib)$(lec).h
	gcc -c $(bib)$(lec).c -o lect.o

psql :  $(bib)$(psql).c $(bib)$(psql).h
	gcc -c $(bib)$(psql).c -o psql.o

norm : $(bib)$(nor).c $(bib)$(nor).h
	gcc -c $(bib)$(nor).c -o norm.o

#Limpieza--------------------------------------------------------------------
clean :
	rm -f *.o *.a
	rm -f implementacion1/*.o implementacion1/*.a
	rm -f implementacion2/*.o implementacion2/*.a
