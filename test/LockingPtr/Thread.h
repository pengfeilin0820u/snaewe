////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter K�mmel
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef LOKI_THREAD_H_
#define LOKI_THREAD_H_

#include <loki/Threads.h>
#include <vector>

#if defined(_WIN32)
 
    #include <process.h>

    typedef unsigned int (WINAPI*ThreadFunction_)(void *);

    #define LOKI_pthread_t HANDLE

    #define LOKI_pthread_create(handle,attr,func,arg) \
        (int)((*handle=(HANDLE) _beginthreadex (NULL,0,(ThreadFunction_)func,arg,0,NULL))==NULL)

    #define LOKI_pthread_join(thread, result) \
        ((WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))

#else

      #define LOKI_pthread_t \
                 pthread_t
    #define LOKI_pthread_create(handle,attr,func,arg) \
                 pthread_create(handle,attr,func,arg)
    #define LOKI_pthread_join(thread, result) \
                 pthread_join(thread, result)

#endif
  
namespace Loki
{


    ////////////////////////////////////////////////////////////////////////////////
    //  \class Thread
    //
    //  \ingroup ThreadingGroup
    //  Very simple Thread class
    ////////////////////////////////////////////////////////////////////////////////
    class Thread
    {
    public:

        typedef void* (*ThreadFunction)(void *);

        Thread(ThreadFunction func, void* parm)
        {
            func_ = func;
            parm_ = parm;
        }

        int start()
        {
            return LOKI_pthread_create(&pthread_, NULL, func_, parm_);
        }

        static int WaitForThread(const Thread& thread)
        {
            int status = 0;
            (void) status;
            return LOKI_pthread_join(thread.pthread_, (void **)&status);
        }

        static void JoinThreads(const std::vector<Thread*>& threads)
        {
            int status = 0;
            (void) status;
            for(size_t i=0; i<threads.size(); i++)
                WaitForThread(*threads.at(i));
        }

        static void DeleteThreads(std::vector<Thread*>& threads)
        {
            for(size_t i=0; i<threads.size(); i++)
            {
                delete threads.at(i);
            }
            threads.clear();
        }

    private:
        LOKI_pthread_t pthread_;
        ThreadFunction func_;
        void* parm_;
    };


    ////////////////////////////////////////////////////////////////////////////////
    //  \class Mutex
    //
    //  \ingroup ThreadingGroup
    //  A portable Mutex
    ////////////////////////////////////////////////////////////////////////////////

    class Mutex
    {
    public:
        Mutex()      { LOKI_THREADS_MUTEX_INIT  (&mtx_);}
        ~Mutex()     { LOKI_THREADS_MUTEX_DELETE(&mtx_);}
        void Lock()  { LOKI_THREADS_MUTEX_LOCK  (&mtx_);}
        void Unlock(){ LOKI_THREADS_MUTEX_UNLOCK(&mtx_);}
    private:
        LOKI_THREADS_MUTEX mtx_;
    };



} // namespace Loki

#endif

// $Log: 
