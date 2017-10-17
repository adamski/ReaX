#pragma once

class Subject::Impl
{
public:
    virtual ~Impl() {}
    virtual rxcpp::subscriber<var> getSubscriber() const = 0;
    virtual rxcpp::observable<var> asObservable() const = 0;
    virtual var getLatestItem() const;
};

class BehaviorSubjectImpl : public Subject::Impl
{
public:
    BehaviorSubjectImpl(const juce::var& initial);

    rxcpp::subscriber<var> getSubscriber() const override;
    rxcpp::observable<var> asObservable() const override;
    var getLatestItem() const override;

private:
    const rxcpp::subjects::behavior<var> wrapped;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BehaviorSubjectImpl)
};

class PublishSubjectImpl : public Subject::Impl
{
public:
    PublishSubjectImpl();

    rxcpp::subscriber<var> getSubscriber() const override;
    rxcpp::observable<var> asObservable() const override;

private:
    const rxcpp::subjects::subject<var> wrapped;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PublishSubjectImpl)
};

class ReplaySubjectImpl : public Subject::Impl
{
public:
    ReplaySubjectImpl(size_t bufferSize);

    rxcpp::subscriber<var> getSubscriber() const override;
    rxcpp::observable<var> asObservable() const override;

private:
    const rxcpp::subjects::replay<var, rxcpp::identity_one_worker> wrapped;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReplaySubjectImpl)
};


















class SubjectBase::Impl
{
public:
    virtual ~Impl() {}
    virtual rxcpp::subscriber<var> getSubscriber() const = 0;
    virtual rxcpp::observable<var> asObservable() const = 0;
    virtual var getLatestItem() const;
};

class TypedBehaviorSubjectImpl : public SubjectBase::Impl
{
public:
    TypedBehaviorSubjectImpl(juce::var&& initial);
    
    rxcpp::subscriber<var> getSubscriber() const override;
    rxcpp::observable<var> asObservable() const override;
    var getLatestItem() const override;
    
private:
    const rxcpp::subjects::behavior<var> wrapped;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TypedBehaviorSubjectImpl)
};

class TypedPublishSubjectImpl : public SubjectBase::Impl
{
public:
    TypedPublishSubjectImpl();
    
    rxcpp::subscriber<var> getSubscriber() const override;
    rxcpp::observable<var> asObservable() const override;
    
private:
    const rxcpp::subjects::subject<var> wrapped;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TypedPublishSubjectImpl)
};

class TypedReplaySubjectImpl : public SubjectBase::Impl
{
public:
    TypedReplaySubjectImpl(size_t bufferSize);
    
    rxcpp::subscriber<var> getSubscriber() const override;
    rxcpp::observable<var> asObservable() const override;
    
private:
    const rxcpp::subjects::replay<var, rxcpp::identity_one_worker> wrapped;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TypedReplaySubjectImpl)
};
