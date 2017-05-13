/*
  ==============================================================================

    rxjuce_Observable.cpp
    Created: 27 Apr 2017 7:07:33am
    Author:  Martin Finke

  ==============================================================================
 */

#include "../RxCpp/Rx/v2/src/rxcpp/rx.hpp"

#include "rxjuce_Observable.h"

#include "rxjuce_Observable_Impl.h"
#include "rxjuce_Observer_Impl.h"
#include "rxjuce_Scheduler_Impl.h"
#include "rxjuce_Subscription_Impl.h"
#include "rxjuce_VariantConverters.h"

RXJUCE_SOURCE_PREFIX

namespace {
	const std::runtime_error InvalidRangeError("Invalid range.");
	
	std::chrono::milliseconds durationFromRelativeTime(const juce::RelativeTime& relativeTime)
	{
		return std::chrono::milliseconds(relativeTime.inMilliseconds());
	}
}


RXJUCE_NAMESPACE_BEGIN

const std::function<void(Error)> Observable::TerminateOnError = [](Error) {
	// error implicitly ignored, abort
	std::terminate();
};

const std::function<void()> Observable::EmptyOnCompleted = [](){};

#pragma mark - Creation

Observable::Observable(const shared_ptr<Impl>& impl)
:	impl(impl) {}

Observable Observable::from(const Array<var>& array)
{
	return Impl::fromRxCpp(rxcpp::observable<>::iterate(array));
}

Observable Observable::fromValue(Value value)
{
	return Impl::fromValue(value);
}

Observable Observable::interval(const juce::RelativeTime& period)
{
	auto o = rxcpp::observable<>::interval(durationFromRelativeTime(period));
	return Impl::fromRxCpp(o.map(toVar<int>));
}

Observable Observable::just(const var& value)
{
	return Impl::fromRxCpp(rxcpp::observable<>::just(value));
}

Observable Observable::range(int first, int last, unsigned int step)
{
	if (first > last)
		throw InvalidRangeError;
	
	auto o = rxcpp::observable<>::range<int>(first, last, step);
	
	return Impl::fromRxCpp(o.map(toVar<int>));
}

Observable Observable::range(double first, double last, unsigned int step)
{
	if (first > last)
		throw InvalidRangeError;
	
	auto o = rxcpp::observable<>::range<double>(first, last, step);
	
	return Impl::fromRxCpp(o.map(toVar<double>));
}

Observable Observable::create(const std::function<void(Observer)>& onSubscribe)
{
	return Impl::fromRxCpp(rxcpp::observable<>::create<var>([onSubscribe](rxcpp::subscriber<var> s) {
		onSubscribe(Observer(std::make_shared<Observer::Impl>(s)));
	}));
}


#pragma mark - Subscription

Subscription Observable::subscribe(const std::function<void(const var&)>& onNext,
								   const std::function<void(Error)>& onError,
								   const std::function<void()>& onCompleted) const
{
	auto subscription = impl->wrapped.subscribe(onNext, onError, onCompleted);
	
	return Subscription(std::make_shared<Subscription::Impl>(subscription));
}

Subscription Observable::subscribe(const std::function<void(const var&)>& onNext,
								   const std::function<void()>& onCompleted,
								   const std::function<void(Error)>& onError) const
{
	return subscribe(onNext, onError, onCompleted);
}


#pragma mark - Operators

Observable Observable::combineLatest(Observable o1, Function2& f) const
{
	return impl->combineLatest(f, o1);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Function3 f) const
{
	return impl->combineLatest(f, o1, o2);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Function4 f) const
{
	return impl->combineLatest(f, o1, o2, o3);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Function5 f) const
{
	return impl->combineLatest(f, o1, o2, o3, o4);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Function6 f) const
{
	return impl->combineLatest(f, o1, o2, o3, o4, o5);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Function7 f) const
{
	return impl->combineLatest(f, o1, o2, o3, o4, o5, o6);
}
Observable Observable::combineLatest(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7, Function8 f) const
{
	return impl->combineLatest(f, o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::concat(Observable o1) const
{
	return impl->concat(o1);
}
Observable Observable::concat(Observable o1, Observable o2) const
{
	return impl->concat(o1, o2);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3) const
{
	return impl->concat(o1, o2, o3);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4) const
{
	return impl->concat(o1, o2, o3, o4);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5) const
{
	return impl->concat(o1, o2, o3, o4, o5);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6) const
{
	return impl->concat(o1, o2, o3, o4, o5, o6);
}
Observable Observable::concat(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7) const
{
	return impl->concat(o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::debounce(const juce::RelativeTime& period) const
{
	return Impl::fromRxCpp(impl->wrapped.debounce(durationFromRelativeTime(period)));
}

Observable Observable::filter(const std::function<bool(const var&)>& predicate) const
{
	return Impl::fromRxCpp(impl->wrapped.filter(predicate));
}

Observable Observable::flatMap(const std::function<Observable(const var&)>& f) const
{
	return Impl::fromRxCpp(impl->wrapped.flat_map([f](const var& value) {
		return f(value).impl->wrapped;
	}));
}

Observable Observable::map(Function1 f) const
{
	return Impl::fromRxCpp(impl->wrapped.map(f));
}

Observable Observable::merge(Observable o1) const
{
	return impl->merge(o1);
}
Observable Observable::merge(Observable o1, Observable o2) const
{
	return impl->merge(o1, o2);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3) const
{
	return impl->merge(o1, o2, o3);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4) const
{
	return impl->merge(o1, o2, o3, o4);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5) const
{
	return impl->merge(o1, o2, o3, o4, o5);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6) const
{
	return impl->merge(o1, o2, o3, o4, o5, o6);
}
Observable Observable::merge(Observable o1, Observable o2, Observable o3, Observable o4, Observable o5, Observable o6, Observable o7) const
{
	return impl->merge(o1, o2, o3, o4, o5, o6, o7);
}

Observable Observable::sample(const juce::RelativeTime& interval)
{
	return Impl::fromRxCpp(impl->wrapped.sample_with_time(durationFromRelativeTime(interval)));
}

Observable Observable::scan(const var& startValue, Function2 f) const
{
	return Impl::fromRxCpp(impl->wrapped.scan(startValue, f));
}

Observable Observable::switchOnNext() const
{
	rxcpp::observable<rxcpp::observable<var>> unwrapped = impl->wrapped.map([](var observable) {
		return fromVar<Observable>(observable).impl->wrapped;
	});
	
	return Impl::fromRxCpp(unwrapped.switch_on_next());
}

Observable Observable::take(unsigned int numItems) const
{
	return Impl::fromRxCpp(impl->wrapped.take(numItems));
}

Observable Observable::takeUntil(Observable other) const
{
	return Impl::fromRxCpp(impl->wrapped.take_until(other.impl->wrapped));
}


#pragma mark - Scheduling

Observable Observable::observeOn(const Scheduler& scheduler) const
{
	return Impl::fromRxCpp(scheduler.impl->schedule(impl->wrapped));
}


#pragma mark - Misc

Observable::operator var() const
{
	return toVar<Observable>(*this);
}

juce::Array<var> Observable::toArray(const std::function<void(Error)>& onError) const
{
	Array<var> items;
	impl->wrapped.as_blocking().subscribe([&](const var& item) {
		items.add(item);
	}, onError);
	return items;
}

RXJUCE_NAMESPACE_END
