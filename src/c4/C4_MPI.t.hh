//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   c4/C4_MPI.t.hh
 * \author Thomas M. Evans
 * \date   Thu Mar 21 16:56:17 2002
 * \brief  C4 MPI template implementation.
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef c4_C4_MPI_t_hh
#define c4_C4_MPI_t_hh

#ifdef C4_MPI

#include "C4_MPI.hh"
#include "C4_Req.hh"
#include "MPI_Traits.hh"
#include <vector>

namespace rtt_c4 {

//---------------------------------------------------------------------------//
// BLOCKING SEND/RECEIVE OPERATIONS
//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 int send(const T *buffer, int size, int destination, int tag) {
  MPI_Send(const_cast<T *>(buffer), size, MPI_Traits<T>::element_type(),
           destination, tag, communicator);
  return C4_SUCCESS;
}

//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 int receive(T *buffer, int size, int source, int tag) {
  int count = 0;

  // get a handle to the MPI_Status
  MPI_Status status;

  // do the blocking receive
  Remember(int check =) MPI_Recv(buffer, size, MPI_Traits<T>::element_type(),
                                 source, tag, communicator, &status);
  Check(check == MPI_SUCCESS);

  // get the count of received data
  MPI_Get_count(&status, MPI_Traits<T>::element_type(), &count);
  return count;
}

//---------------------------------------------------------------------------------------//
template <typename T>
DLL_PUBLIC_c4 int send_udt(const T *buffer, int size, int destination,
                           C4_Datatype &data_type, int tag) {
  MPI_Send(const_cast<T *>(buffer), size, data_type, destination, tag,
           communicator);
  return C4_SUCCESS;
}

//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 int receive_udt(T *buffer, int size, int source,
                              C4_Datatype &data_type, int tag) {
  int count = 0;

  // get a handle to the MPI_Status
  MPI_Status status;

  // do the blocking receive
  MPI_Recv(buffer, size, data_type, source, tag, communicator, &status);

  // get the count of received data
  MPI_Get_count(&status, data_type, &count);
  return count;
}

//---------------------------------------------------------------------------//
// NON-BLOCKING SEND/RECEIVE OPERATIONS
//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 C4_Req send_async(const T *buffer, int size, int destination,
                                int tag) {
  // make a c4 request handle
  C4_Req request;

  // do an MPI_Isend (non-blocking send)
  Remember(int const retval =)
      MPI_Isend(const_cast<T *>(buffer), size, MPI_Traits<T>::element_type(),
                destination, tag, communicator, &request.r());
  Check(retval == MPI_SUCCESS);

  // set the request to active
  request.set();
  return request;
}

//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 void send_async(C4_Req &request, const T *buffer, int size,
                              int destination, int tag) {
  Require(!request.inuse());

  // set the request
  request.set();

  // post an MPI_Isend
  MPI_Isend(const_cast<T *>(buffer), size, MPI_Traits<T>::element_type(),
            destination, tag, communicator, &request.r());
}

//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 void send_is(C4_Req &request, const T *buffer, int size,
                           int destination, int tag) {
  Require(!request.inuse());

  // set the request
  request.set();

  Remember(int const retval =)
      MPI_Issend(const_cast<T *>(buffer), size, MPI_Traits<T>::element_type(),
                 destination, tag, communicator, &request.r());
  Check(retval == MPI_SUCCESS);

  return;
}

//---------------------------------------------------------------------------//

template <typename T>
C4_Req receive_async(T *buffer, int size, int source, int tag) {
  // make a c4 request handle
  C4_Req request;

  // post an MPI_Irecv (non-blocking receive)
  Remember(int const retval =)
      MPI_Irecv(buffer, size, MPI_Traits<T>::element_type(), source, tag,
                communicator, &request.r());
  Check(retval == MPI_SUCCESS);

  // set the request to active
  request.set();
  return request;
}

//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 void receive_async(C4_Req &request, T *buffer, int size,
                                 int source, int tag) {
  Require(!request.inuse());

  // set the request
  request.set();

  // post an MPI_Irecv
  Remember(int const retval =)
      MPI_Irecv(buffer, size, MPI_Traits<T>::element_type(), source, tag,
                communicator, &request.r());
  Check(retval == MPI_SUCCESS);
  return;
}

//---------------------------------------------------------------------------//
// BROADCAST
//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 int broadcast(T *buffer, int size, int root) {
  int r = MPI_Bcast(buffer, size, MPI_Traits<T>::element_type(), root,
                    communicator);
  return r;
}

//---------------------------------------------------------------------------//
// GATHER/SCATTER
//---------------------------------------------------------------------------//

template <typename T>
DLL_PUBLIC_c4 int gather(T *send_buffer, T *receive_buffer, int size) {
  int Result = MPI_Gather(send_buffer, size, MPI_Traits<T>::element_type(),
                          receive_buffer, size, MPI_Traits<T>::element_type(),
                          0, // root is always processor 0 at present
                          communicator);

  return Result;
}

template <typename T>
DLL_PUBLIC_c4 int allgather(T *send_buffer, T *receive_buffer, int size) {
  int Result = MPI_Allgather(send_buffer, size, MPI_Traits<T>::element_type(),
                             receive_buffer, size,
                             MPI_Traits<T>::element_type(), communicator);

  return Result;
}

template <typename T>
DLL_PUBLIC_c4 int scatter(T *send_buffer, T *receive_buffer, int size) {
  int Result = MPI_Scatter(send_buffer, size, MPI_Traits<T>::element_type(),
                           receive_buffer, size, MPI_Traits<T>::element_type(),
                           0, // root is always processor 0 at present
                           communicator);

  return Result;
}

template <typename T>
int gatherv(T *send_buffer, int send_size, T *receive_buffer,
            int *receive_sizes, int *receive_displs) {
  int Result = MPI_Gatherv(
      send_buffer, send_size, MPI_Traits<T>::element_type(), receive_buffer,
      receive_sizes, receive_displs, MPI_Traits<T>::element_type(),
      0, // root is always processor 0 at present
      communicator);

  return Result;
}

template <typename T>
int scatterv(T *send_buffer, int *send_sizes, int *send_displs,
             T *receive_buffer, int receive_size) {
  int Result =
      MPI_Scatterv(send_buffer, send_sizes, send_displs,
                   MPI_Traits<T>::element_type(), receive_buffer, receive_size,
                   MPI_Traits<T>::element_type(), 0, communicator);

  return Result;
}

//---------------------------------------------------------------------------//
// GLOBAL REDUCTIONS
//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_sum(T &x) {
  // copy data into send buffer
  T y = x;

  // do global MPI reduction (result is on all processors) into x
  Remember(int check =) MPI_Allreduce(&y, &x, 1, MPI_Traits<T>::element_type(),
                                      MPI_SUM, communicator);
  Check(check == MPI_SUCCESS);
  return;
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_prod(T &x) {
  // copy data into send buffer
  T y = x;

  // do global MPI reduction (result is on all processors) into x
  MPI_Allreduce(&y, &x, 1, MPI_Traits<T>::element_type(), MPI_PROD,
                communicator);
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_min(T &x) {
  // copy data into send buffer
  T y = x;

  // do global MPI reduction (result is on all processors) into x
  MPI_Allreduce(&y, &x, 1, MPI_Traits<T>::element_type(), MPI_MIN,
                communicator);
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_max(T &x) {
  // copy data into send buffer
  T y = x;

  // do global MPI reduction (result is on all processors) into x
  MPI_Allreduce(&y, &x, 1, MPI_Traits<T>::element_type(), MPI_MAX,
                communicator);
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_sum(T *x, int n) {
  // copy data into a send buffer
  std::vector<T> send_buffer(x, x + n);

  // do a element-wise global reduction (result is on all processors) into
  // x
  MPI_Allreduce(&send_buffer[0], x, n, MPI_Traits<T>::element_type(), MPI_SUM,
                communicator);
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_prod(T *x, int n) {
  // copy data into a send buffer
  std::vector<T> send_buffer(x, x + n);

  // do a element-wise global reduction (result is on all processors) into
  // x
  MPI_Allreduce(&send_buffer[0], x, n, MPI_Traits<T>::element_type(), MPI_PROD,
                communicator);
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_min(T *x, int n) {
  // copy data into a send buffer
  std::vector<T> send_buffer(x, x + n);

  // do a element-wise global reduction (result is on all processors) into
  // x
  MPI_Allreduce(&send_buffer[0], x, n, MPI_Traits<T>::element_type(), MPI_MIN,
                communicator);
}

//---------------------------------------------------------------------------//

template <typename T> DLL_PUBLIC_c4 void global_max(T *x, int n) {
  // copy data into a send buffer
  std::vector<T> send_buffer(x, x + n);

  // do a element-wise global reduction (result is on all processors) into
  // x
  MPI_Allreduce(&send_buffer[0], x, n, MPI_Traits<T>::element_type(), MPI_MAX,
                communicator);
}

} // end namespace rtt_c4

#endif // C4_MPI

#endif // c4_C4_MPI_t_hh

//---------------------------------------------------------------------------//
// end of c4/C4_MPI.t.hh
//---------------------------------------------------------------------------//
