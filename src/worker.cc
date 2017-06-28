#include "worker.h"

#include "nan.h"
#include "spellchecker.h"

#include <string>
#include <vector>
#include <utility>

CheckSpellingWorker::CheckSpellingWorker(
  const std::vector<uint16_t>&& corpus,
  SpellcheckerImplementation* impl,
  Nan::Callback* callback
) : AsyncWorker(callback), corpus(std::move(corpus)), impl(impl)
{
  // No-op
}

CheckSpellingWorker::~CheckSpellingWorker()
{
  // No-op
}

void CheckSpellingWorker::Execute() {
  misspelled_ranges = impl->CheckSpelling(corpus.data(), corpus.size());
}

void CheckSpellingWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  Local<Array> result = Nan::New<Array>();
  for (auto iter = misspelled_ranges.begin(); iter != misspelled_ranges.end(); ++iter) {
    size_t index = iter - misspelled_ranges.begin();
    uint32_t start = iter->start, end = iter->end;

    Local<Object> misspelled_range = Nan::New<Object>();
    misspelled_range->Set(Nan::New("start").ToLocalChecked(), Nan::New<Integer>(start));
    misspelled_range->Set(Nan::New("end").ToLocalChecked(), Nan::New<Integer>(end));
    result->Set(index, misspelled_range);
  }

  Local<Value> argv[] = { Nan::Null(), result };
  callback->Call(2, argv);
}
