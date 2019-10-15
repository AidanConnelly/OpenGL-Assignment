Operations needed:

 * PARALLEL MAP
 * PARALLEL REDUCE
 * SERIAL VECTORISED OPERATIONS
 * SERIAL NONVECTORISED OPERATIONS
 * PARALLEL FILTER
 * SYNCHRONISE
 * SERIAL REDUCE
 * SERIAL MAP

Example:
(A)-->[D]-->[E]-->(SYNCHRONISE)

[PARALLEL OPERATION]
(SERIAL OPERATION)

The controlling thead's code should look something like:

for(int i = 0;i< nClient;i++) {
    toDo[i] = actionCode.chainDE;
    startDoing[i] = 1;
}

The worker thread's code should look something like:

while(true){
    if(startDoing[i]){
        resolveAndDo(i);
    }
    else{
        sleep;
    }
}

with resolveAndDo being a giant switch on an int, as function pointers or VLTs will probably be too slow.

Threads should be able to access shared memory only.

