      PROGRAM helloworld
C     cloc -c -fort  will generate launch_params.f so you can set dimensions  
      INCLUDE 'launch_params.f'

      INTEGER :: i,lmsg 
      CHARACTER(LEN=*), PARAMETER :: msg = "Hello HSA World"
      CHARACTER inputg(*), secret(*), output(*)
      pointer(s_ptr,secret)
      pointer(o_ptr,output)
      pointer(i_ptr,inputg)
C     Malloc memory to use in GPU functions
      s_ptr = malloc_global(64)
      o_ptr = malloc_global(64)
      i_ptr = malloc_global(64)
     
      lmsg=LEN(msg)
      DO i=1,lmsg
         inputg(i:i) = msg(i:i)
      END DO

C     Initialize the grid dimensions defined in the launch_params.f file
      lparm%ndim=1 
      lparm%gdims(1)=lmsg
      lparm%ldims(1)=1

C     Call the GPU functions
C     Must use HSA global register memory 
      CALL encode(inputg,secret,lparm);
      PRINT*, "Coded message  :",secret(1:lmsg)
      CALL decode(secret,output,lparm);
      PRINT*, "Decoded message:",output(1:lmsg)
      END
