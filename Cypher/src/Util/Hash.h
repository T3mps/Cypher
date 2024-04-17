#include "picosha2.h"

#include "Types.h"

namespace Cypher
{
   static const size_t DIGEST_SIZE = picosha2::k_digest_size;

   template <typename InIter, typename OutContainer>
   void Hash256(const InIter& first, const InIter& last, OutContainer& dst) { picosha2::hash256(first, last, dst); }
   
   template <typename InIter, typename OutIter>
   void Hash256(InIter first, InIter last, OutIter first2, OutIter last2) { picosha2::hash256(first, last, first2, last2); }

   std::string Hash256(const std::string& input)
   {
      std::string hash_hex_str;
      picosha2::hash256_hex_string(input.begin(), input.end(), hash_hex_str);
      return hash_hex_str;
   }
   
   template <typename InIter>
   std::string BytesToHexString(InIter first, InIter last) { return picosha2::bytes_to_hex_string(first, last); }
}
