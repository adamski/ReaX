/*
  ==============================================================================

    rxjuce_Observer.cpp
    Created: 27 Apr 2017 7:08:56am
    Author:  Martin Finke

  ==============================================================================
*/

#include "rxjuce_Observer.h"

#include "rxjuce_Observer_Impl.h"

RXJUCE_SOURCE_PREFIX

RXJUCE_NAMESPACE_BEGIN

Observer::Observer(const std::shared_ptr<Impl>& impl)
: impl(impl) {}

void Observer::onNext(const juce::var& next) const
{
	impl->wrapped.on_next(next);
}

RXJUCE_NAMESPACE_END
