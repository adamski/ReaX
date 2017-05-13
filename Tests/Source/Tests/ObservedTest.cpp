/*
  ==============================================================================

    ReactiveTest.cpp
    Created: 28 Apr 2017 8:12:02pm
    Author:  Martin Finke

  ==============================================================================
*/

#include "rxjuce_TestPrefix.h"

#include "rxjuce_Reactive.h"
#include "rxjuce_VariantConverters.h"


TEST_CASE("Reactive<Button> stateChanged",
		  "[Reactive<Button>]")
{
	Reactive<TextButton> button("Click Here");
	Array<var> items;
	RxJUCECollectItems(button.rx.buttonState, items);
	
	IT("emits the normal state on subscribe") {
		RxJUCERequireItems(items, Button::ButtonState::buttonNormal);
	}
	
	IT("emits items synchronously when the Button state changes") {
		button.setState(Button::ButtonState::buttonDown);
		
		RxJUCECheckItems(items,
						 Button::ButtonState::buttonNormal,
						 Button::ButtonState::buttonDown);
		
		button.setState(Button::ButtonState::buttonNormal);
		button.setState(Button::ButtonState::buttonOver);
		
		RxJUCERequireItems(items,
						   Button::ButtonState::buttonNormal,
						   Button::ButtonState::buttonDown,
						   Button::ButtonState::buttonNormal,
						   Button::ButtonState::buttonOver);
	}
}


TEST_CASE("Reactive<Button> clicked",
		  "[Reactive<Button>]")
{
	Reactive<TextButton> button("Click Here");
	Array<var> items;
	RxJUCECollectItems(button.rx.clicked, items);
	
	IT("doesn't emit an item on subscribe") {
		REQUIRE(items.isEmpty());
	}
	
	IT("emits void vars asynchronously when the Button is clicked") {
		button.triggerClick();
		RxJUCERunDispatchLoop();
		
		RxJUCECheckItems(items, var::undefined());
		
		button.triggerClick();
		button.triggerClick();
		RxJUCERunDispatchLoop();
		
		RxJUCERequireItems(items, var::undefined(), var::undefined(), var::undefined());
	}
}


TEST_CASE("Reactive<Button> with custom TextButton subclass",
		  "[Reactive<Button>]")
{
	class MyButton : public TextButton
	{
	public:
		void hoverAcrossButton()
		{
			setState(buttonOver);
			MessageManager::getInstance()->callAsync([this]() {
				setState(buttonNormal);
			});
		}
	};
	
	Reactive<MyButton> button;
	Array<var> items;
	RxJUCECollectItems(button.rx.buttonState, items);
	
	IT("initially has the normal state") {
		RxJUCERequireItems(items, Button::ButtonState::buttonNormal);
	}
	
	IT("changes states when calling the method in the custom subclass") {
		button.hoverAcrossButton();
		RxJUCECheckItems(items,
						 Button::ButtonState::buttonNormal,
						 Button::ButtonState::buttonOver);
		RxJUCERunDispatchLoop();
		
		RxJUCERequireItems(items,
						   Button::ButtonState::buttonNormal,
						   Button::ButtonState::buttonOver,
						   Button::ButtonState::buttonNormal);
	}
}


TEST_CASE("Reactive<Value> conversion",
		  "[Reactive<Value>]")
{
	Reactive<Value> value;
	
	IT("supports copy assignment from var-compatible types") {
		value = 3;
		value = Array<var>({6, 7, 5});
		value = "Some String";
		REQUIRE(value.getValue() == "Some String");
	}
	
	IT("can be implicitly converted to var") {
		value.setValue("Testing");
		var v = value;
		REQUIRE(v == "Testing");
	}
	
	IT("supports == and != with var-compatible types") {
		value.setValue("Hello!");
		REQUIRE(value == "Hello!");
		REQUIRE(value != "World");
		REQUIRE(value != 3.45);
		REQUIRE(value != 2);
	}
}


TEST_CASE("Reactive<Value> Observable",
		  "[Reactive<Value>]")
{
	auto value = std::make_shared<Reactive<Value>>("Initial");
	Array<var> items;
	RxJUCECollectItems(value->rx.subject, items);
	
	IT("emits items asynchronously when the Value changes") {
		value->setValue("Second");
		RxJUCECheckItems(items, "Initial");
		RxJUCERunDispatchLoop();
		RxJUCECheckItems(items, "Initial", "Second");
		value->setValue("Third");
		RxJUCERunDispatchLoop();
		
		RxJUCERequireItems(items, "Initial", "Second", "Third");
	}
	
	IT("stops emitting items immediately when being destroyed") {
		value->setValue("Should not arrive");
		value.reset();
		RxJUCERunDispatchLoop();
		
		RxJUCERequireItems(items, "Initial");
	}
}

TEST_CASE("ComponentConnection")
{
	Array<var> items;
	Reactive<Component> component;
	RxJUCECollectItems(component.rx.visible, items);
	
	IT("initially has the same value as the getter") {
		REQUIRE(component.isVisible() == fromVar<bool>(component.rx.visible.getLatestItem()));
	}
	
	IT("emits when visibility is changed through setter") {
		for (bool visible : {false, false, true, true, false}) {
			component.setVisible(visible);
		}
		
		RxJUCERequireItems(items, false, true, false);
	}
	
	IT("changes visiblility when pushing items") {
		for (bool visible : {false, false, true, true, false}) {
			component.rx.visible.onNext(visible);
			
			REQUIRE(component.isVisible() == visible);
		}
	}
}


template<typename T1, typename T2>
using isSame = typename std::is_same<typename std::decay<T1>::type, T2>;

TEST_CASE("Template ambiguities")
{
	Array<var> items;
	
	IT("chooses the correct template for a juce::Component") {
		Reactive<Component> myComponent;
		static_assert(isSame<decltype(myComponent.rx), ComponentConnection>::value, "rx Member has wrong type.");
	}
	
	IT("chooses the correct template for a Component subclass") {
		class MyCustomComponent : public Component {};
		Reactive<MyCustomComponent> myCustomComponent;
		static_assert(isSame<decltype(myCustomComponent.rx), ComponentConnection>::value, "rx Member has wrong type.");
	}
	
	IT("chooses the correct template for a Button subclass") {
		class MyButton : public Button {
		public:
			MyButton()
			: Button("") {}
			void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override {}
		};
		Reactive<MyButton> myButton;
		static_assert(isSame<decltype(myButton.rx), ButtonConnection>::value, "rx Member has wrong type.");
	}
	
	IT("chooses the correct template for a juce::ImageComponent") {
		Reactive<ImageComponent> myImageComponent;
		static_assert(isSame<decltype(myImageComponent.rx), ImageComponentConnection>::value, "rx Member has wrong type.");
	}
}
