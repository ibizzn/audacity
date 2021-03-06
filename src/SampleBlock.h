/**********************************************************************

Audacity: A Digital Audio Editor

SampleBlock.h

**********************************************************************/

#ifndef __AUDACITY_SAMPLE_BLOCK__
#define __AUDACITY_SAMPLE_BLOCK__

#include "ClientData.h" // to inherit

#include <functional>
#include <memory>

class AudacityProject;
class ProjectFileIO;
class XMLWriter;

class SampleBlock;
using SampleBlockPtr = std::shared_ptr<SampleBlock>;
class SampleBlockFactory;
using SampleBlockFactoryPtr = std::shared_ptr<SampleBlockFactory>;
using SampleBlockFactoryFactory =
   std::function< SampleBlockFactoryPtr( AudacityProject& ) >;

//using SampleBlockID = sqlite3_int64; // Trying not to depend on sqlite headers
using SampleBlockID = long long;

class MinMaxRMS
{
public:
   float min;
   float max;
   float RMS;
};

class SqliteSampleBlockFactory;

///\brief Abstract class allows access to contents of a block of sound samples,
/// serialization as XML, and reference count management that can suppress
/// reclamation of its storage
class SampleBlock
{
public:
   virtual ~SampleBlock();

   virtual void Lock() = 0;
   virtual void Unlock() = 0;
   virtual void CloseLock() = 0;
   
   virtual SampleBlockID GetBlockID() = 0;

   virtual size_t GetSamples(samplePtr dest,
                     sampleFormat destformat,
                     size_t sampleoffset,
                     size_t numsamples) = 0;

   virtual size_t GetSampleCount() const = 0;

   virtual bool
      GetSummary256(float *dest, size_t frameoffset, size_t numframes) = 0;
   virtual bool
      GetSummary64k(float *dest, size_t frameoffset, size_t numframes) = 0;

   /// Gets extreme values for the specified region
   virtual MinMaxRMS GetMinMaxRMS(size_t start, size_t len) = 0;

   /// Gets extreme values for the entire block
   virtual MinMaxRMS GetMinMaxRMS() const = 0;

   virtual size_t GetSpaceUsage() const = 0;

   virtual void SaveXML(XMLWriter &xmlFile) = 0;
};

///\brief abstract base class with methods to produce @ref SampleBlock objects
class SampleBlockFactory
{
public:
   // Install global function that produces a sample block factory object for
   // a given project; the factory has methods that later make sample blocks.
   // Return the previously installed factory.
   static SampleBlockFactoryFactory RegisterFactoryFactory(
      SampleBlockFactoryFactory newFactory );

   // Invoke the installed factory (throw an exception if none was installed)
   static SampleBlockFactoryPtr New( AudacityProject &project );

   virtual ~SampleBlockFactory();

   virtual SampleBlockPtr Get(SampleBlockID sbid) = 0;

   virtual SampleBlockPtr Create(samplePtr src,
      size_t numsamples,
      sampleFormat srcformat) = 0;

   virtual SampleBlockPtr CreateSilent(
      size_t numsamples,
      sampleFormat srcformat) = 0;

   virtual SampleBlockPtr CreateFromXML(
      sampleFormat srcformat,
      const wxChar **attrs) = 0;
};

#endif
