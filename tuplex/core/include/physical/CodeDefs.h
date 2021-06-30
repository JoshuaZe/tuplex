//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                      Tuplex: Blazing Fast Python Data Science                                      //
//                                                                                                                    //
//                                                                                                                    //
//  (c) 2017 - 2021, Tuplex team                                                                                      //
//  Created by Leonhard Spiegelberg first on 1/1/2021                                                                 //
//  License: Apache 2.0                                                                                               //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef TUPLEX_CODEDEFS_H
#define TUPLEX_CODEDEFS_H

#include <cstdint>

namespace tuplex {

    namespace codegen {
        // (1) typedefs
        // for building customizable/hackable transform stages with code generated task code (One task = iterating over one partition)
        // several functions are needed. Following are some typedefs to allow for easier handling


        // @deprecated, remove/?
        //! type of the function to request a new output partition
        //! takes pointer to userdata, a minimum requested memory from the function at this point and
        //! a pointer where the function needs to write how many bytes it actually allocated
        //! returns pointer to memory location where task function can write data to. MUST HAVE capacity
        //! as returned via 3rd parameter.
        typedef uint8_t*(*reqMemory_f)(void*, int64_t, int64_t*);


        //! type of exception handler function, handler is called whenever exception occurs in UDF code
        //! parameters are as follows (read top-bottom like left-right)
        //! void*       userData                pointer to some userData passed down via the task function,
        //!                                     need to give her input partition etc. for error handling
        //! int64_t     exceptionCode           exceptionCode (specified via enum)
        //! int64_t     exceptionOperatorID     the ID of the (logical) operator to which the UDF belongs
        //! int64_t     rowNumber               within Task/Partition, starts at 0
        //! uint8_t*    input                   pointer to memory location of data that caused exception
        //! int64_t     dataLength              how many bytes the input data is long, can be used
        //!                                     to easily copy out memory region
        typedef void(*exception_handler_f)(void*, int64_t, int64_t, int64_t, uint8_t*, int64_t);


        // prototype of the function generated by the below builder
        // parameters are userData, array of globals (e.g. hashmaps), block, blocksize, normalrowsout, badrowsout, lastRow
        typedef int64_t(*read_block_f)(void*, const uint8_t*, int64_t, int64_t*, int64_t*, int8_t);

        /*!
         * prototype for processing a single row (with callbacks etc.). Returns how many bytes were processed
         */
        typedef int64_t(*process_row_f)(void*, const uint8_t*, int64_t, int64_t);

        // prototype for a writing call back
        // it gets userdata
        // then the buffer in whatever format was requested
        // and the size of the buffer to write
        // return value should be an exception code. framework will save then problematic output row or not
        typedef int64_t(*write_row_f)(void*, const uint8_t*, int64_t);

        // functor accepting various csv cells
        // i.e. first arg is userData, then globals, then comes row number(for exceptions), then two arrays:
        // first one holds each cell start, second one it length of cell.
        // ==> practical for parsing CSV/JSON!
        typedef int64_t(*cells_row_f)(void*, int64_t, char **, int64_t*);

        // functions used when hashing a row in build phase.
        // 1. user data as usual,
        // 2. str key/int key
        // 3. str key length/bool key_null
        // 4. bool bucketize - indicates whether or not to extend the bucket
        // 5. bucket
        // 6. bucket_size
        // note that key or bucket may be nullptr for option types!
        typedef void(*str_hash_row_f)(void*, const uint8_t* str_key, int64_t str_key_size, bool bucketize, const uint8_t* bucket, int64_t bucket_size);
        typedef void(*i64_hash_row_f)(void*, int64_t int_key, bool int_key_null, bool bucketize, const uint8_t* bucket, int64_t bucket_size);

        // // functor to initialize/release the global variables. ==> call in init stage!
        // typedef int64_t(*global_init_release_f)();

        // this is a pair to initialize a stage and free the resources
        // => handles passing hashmaps, + setting up shared objects
        typedef int64_t(*init_stage_f)(int64_t, void**, void**);
        typedef int64_t(*release_stage_f)();

        // resolve functor type
        // (int64_t)(*)(void* userData, int64_t rowNumber, int64_t ExceptionCode, uint8_t* inputBuffer, int64_t inputBufferSize)
        typedef int64_t(*resolve_f)(void*, int64_t, int64_t, const uint8_t*, int64_t);

        // aggregate funcs

        // int64_t agg_init_f(uint8_t** init_val, int64_t* init_size) -> this functor will allocate + populate the initial
        // value, and return the location/size of the buffer in the arguments
        typedef int64_t(*agg_init_f)(uint8_t**,int64_t*);

        // int64_t agg_combine_f(uint8_t** valA, int64_t* sizeA, uint8_t* valB, int64_t sizeB)
        //  => combines the passed in values, and places the combined value in valA (it will reallocate and rewrite
        //     valA, sizeA if more space is needed)
        typedef int64_t(*agg_combine_f)(uint8_t**,int64_t*,uint8_t*,int64_t);

        // int64_t agg_agg_f(uint8_t** valA, uint8_t* valB, int64_t sizeB)
        //  => aggregates the passed in values, and places the aggregate value in valA (it will reallocate and rewrite
        //     valA if more space is needed)
        //     NOTE: valA has the in-memory format: size | val (size is 64-bit)
        typedef int64_t(*agg_agg_f)(uint8_t**,uint8_t*,int64_t);
    }
}

#endif //TUPLEX_CODEDEFS_H