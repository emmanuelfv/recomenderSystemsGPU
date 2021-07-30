#Ejemplo de  archivo Makefile (Reglas implícitas)
lec = lecturaRS
nor = normalizacion
psql = lecturaPSQL
valid = validacion
ik = itemKNN
bib = ../bibliotecas/

#fun_knn: ik3

#programas
validacion: validation01.c basicRS
	gcc -static -o validacion.o validation01.c -lm -L. -lBasicRS 
	#El orden en que se desecciona un directorio de búsqueda (con -L) importa,
	#puede estar presente -L varias veces. 

ik1: itemKNN_V1.c basicRS
	gcc $(ik)_V1.c -o $(ik)_V1.o -lm -lpq -L. -lBasicRS

ik2: itemKNN_V2.c basicRS
	gcc $(ik)_V2.c -o $(ik)_V2.o -lm -lpq -L. -lBasicRS

ik3: itemKNN_V3.c basicRS
	gcc $(ik)_V3.c -o $(ik)_V3.o -lm -lpq -L. -lBasicRS


#GPU
ikg1: itemKNN_GPU_V1.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V1.cu -o $(ik)_GPU_V1.o -lm

ikg2: itemKNN_GPU_V2.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V2.cu -o $(ik)_GPU_V2.o -lm

ikg3: itemKNN_GPU_V3.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V3.cu -o $(ik)_GPU_V3.o -lm

ikg4: itemKNN_GPU_V4.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V4.cu -o $(ik)_GPU_V4.o -lm -lpq

ikg5: itemKNN_GPU_V5.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V5.cu -o $(ik)_GPU_V5.o -lm

ikg6: itemKNN_GPU_V6.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V6.cu -o $(ik)_GPU_V6.o -lm -lpq

ikg7: itemKNN_GPU_V7.cu basicRS
	nvcc -arch sm_75 $(ik)_GPU_V7.cu -o $(ik)_GPU_V7.o -lm -lpq

ikg: itemKNN_GPU.cu $(bib)$(lec).c $(bib)$(psql).c $(bib)$(nor).c
	cp $(bib)$(lec).c $(bib)$(lec).cu
	cp $(bib)$(psql).c $(bib)$(psql).cu
	cp $(bib)$(nor).c $(bib)$(nor).cu
	nvcc --gpu-architecture=sm_75 --device-c $(ik)_GPU.cu $(bib)$(lec).cu $(bib)$(psql).cu $(bib)$(nor).cu
	nvcc --gpu-architecture=sm_75 -o ItemKNN.o $(ik)_GPU.o $(lec).o $(psql).o $(nor).o -lpq
	rm $(bib)*.cu

#pruebas
t1p1: t1p1.c basicRS
	gcc t1p1.c -o t1p1.o -lm -lpq -L. -lBasicRS

t1p2: t1p2.c basicRS
	gcc t1p2.c -o t1p2.o -lm -lpq -L. -lBasicRS

t1p3: t1p3.c basicRS
	gcc t1p3.c -o t1p3.o -lm -lpq -L. -lBasicRS

p1: p1.cu $(bib)$(lec).c $(bib)$(psql).c
	cp $(bib)$(lec).c $(bib)$(lec).cu
	cp $(bib)$(psql).c $(bib)$(psql).cu
	nvcc --gpu-architecture=sm_75 --device-c p1.cu $(bib)$(lec).cu $(bib)$(psql).cu
	nvcc --gpu-architecture=sm_75 -o prueba.o p1.o $(lec).o $(psql).o -lpq
	rm $(bib)*.cu


	

#bibliotecas
basicRS: lect psql norm valid
	ar rcs libBasicRS.a lect.o psql.o norm.o valid.o


#modulos
lect :  $(bib)$(lec).c $(bib)$(lec).h
	gcc -c $(bib)$(lec).c -o lect.o

psql :  $(bib)$(psql).c $(bib)$(psql).h
	gcc -c $(bib)$(psql).c -o psql.o

norm : $(bib)$(nor).c $(bib)$(nor).h
	gcc -c $(bib)$(nor).c -o norm.o

valid : $(bib)$(valid).c $(bib)$(valid).h
	gcc -c $(bib)$(valid).c -o valid.o

clean :
	rm -f *.o *.a
