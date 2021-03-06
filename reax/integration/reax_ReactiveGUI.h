#pragma once

namespace detail {
template<typename T>
using IsSimpleComponent = typename std::enable_if<std::is_base_of<juce::Component, T>::value && !std::is_base_of<juce::ImageComponent, T>::value && !std::is_base_of<juce::Button, T>::value && !std::is_base_of<juce::Label, T>::value && !std::is_base_of<juce::Slider, T>::value>::type;

template<typename T>
using IsImageComponent = typename std::enable_if<std::is_base_of<juce::ImageComponent, T>::value>::type;

template<typename T>
using IsButton = typename std::enable_if<std::is_base_of<juce::Button, T>::value>::type;

template<typename T>
using IsLabel = typename std::enable_if<std::is_base_of<juce::Label, T>::value>::type;

template<typename T>
using IsSlider = typename std::enable_if<std::is_base_of<juce::Slider, T>::value>::type;
}

/**
    Adds reactive extensions to a `Component` (or subclass).
 */
template<typename ComponentType>
class Reactive<ComponentType, detail::IsSimpleComponent<ComponentType>> : public ComponentType
{
public:
    /// Creates a new instance. @see `juce::Component::Component`.
    template<typename... Args>
    Reactive(Args&&... args)
    : ComponentType(std::forward<Args>(args)...),
      rx(*this)
    {}

    /// The reactive extension object. 
    const ComponentExtension rx;
};

/**
    Adds reactive extensions to an `ImageComponent` (or subclass).
 */
template<typename ImageComponentType>
class Reactive<ImageComponentType, detail::IsImageComponent<ImageComponentType>> : public ImageComponentType
{
public:
    /// Creates a new instance. @see `juce::ImageComponent::ImageComponent`.
    template<typename... Args>
    Reactive(Args&&... args)
    : ImageComponentType(std::forward<Args>(args)...),
      rx(*this)
    {}

    /// The reactive extension object. 
    const ImageComponentExtension rx;
};

/**
    Adds reactive extensions to a `Button` (or subclass).
 */
template<typename ButtonType>
class Reactive<ButtonType, detail::IsButton<ButtonType>> : public ButtonType
{
public:
    /// Creates a new instance. @see `juce::Button::Button`.
    template<typename... Args>
    Reactive(Args&&... args)
    : ButtonType(std::forward<Args>(args)...),
      rx(*this)
    {}

    /// The reactive extension object. 
    const ButtonExtension rx;
};

/**
    Adds reactive extensions to a `Label` (or subclass).
 */
template<typename LabelType>
class Reactive<LabelType, detail::IsLabel<LabelType>> : public LabelType
{
public:
    /// Creates a new instance. @see `juce::Label::Label`.
    template<typename... Args>
    Reactive(Args&&... args)
    : LabelType(std::forward<Args>(args)...),
      rx(*this)
    {}

    /// The reactive extension object. 
    const LabelExtension rx;
};

/**
    Adds reactive extensions to a `Slider` (or subclass).
 */
template<typename SliderType>
class Reactive<SliderType, detail::IsSlider<SliderType>> : public SliderType
{
    typedef std::function<double(const juce::String&)> GetValueFromText_Function;
    typedef std::function<juce::String(double)> GetTextFromValue_Function;

    GetValueFromText_Function getValueFromText_Function;
    GetTextFromValue_Function getTextFromValue_Function;

    PublishSubject<GetValueFromText_Function> getValueFromText_Subject;
    PublishSubject<GetTextFromValue_Function> getTextFromValue_Subject;

public:
    /// Creates a new instance. @see `juce::Slider::Slider`. 
    template<typename... Args>
    Reactive(Args&&... args)
    : SliderType(std::forward<Args>(args)...),
      rx(*this, getValueFromText_Subject, getTextFromValue_Subject)
    {
        getValueFromText_Subject.subscribe([this](const GetValueFromText_Function& function) {
            this->getValueFromText_Function = function;
        });

        getTextFromValue_Subject.subscribe([this](const GetTextFromValue_Function& function) {
            this->getTextFromValue_Function = function;
            this->updateText();
        });
    }

    /// The reactive extension object. 
    const SliderExtension rx;

    ///@cond INTERNAL
    double getValueFromText(const juce::String& text) override
    {
        if (getValueFromText_Function)
            return getValueFromText_Function(text);
        else
            return SliderType::getValueFromText(text);
    }

    juce::String getTextFromValue(double value) override
    {
        if (getTextFromValue_Function)
            return getTextFromValue_Function(value);
        else
            return SliderType::getTextFromValue(value);
    }
    ///@endcond
};
