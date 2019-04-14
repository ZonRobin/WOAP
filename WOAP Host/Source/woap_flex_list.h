#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_look_and_feel.h"

namespace WOAP
{
	namespace GUI
	{
		template <class ItemType>
		class FlexList : public Viewport, public DragAndDropTarget
		{
		public:
			enum class Direction
			{
				Horizontal,
				Vertical
			};

			class Listener
			{
			public:
				virtual void swapGestureCallback(FlexList<ItemType>* list, int sourceIndex, int targetIndex) = 0;
				virtual void clickedToEmptySpace(FlexList<ItemType>*, const MouseEvent&) {}
			};

			class DropCursorComponent : public Component
			{
			public:
				DropCursorComponent() { colour = CustomColours::green.withAlpha(0.7f); }
				~DropCursorComponent() {}

				void paint(Graphics& g) override
				{
					g.setColour(colour);
					g.fillRoundedRectangle(getLocalBounds().toFloat(), 1.0f);
				}

				Colour colour;
			};

			FlexList(Direction d)
				: direction(d)
			{
				setViewportIgnoreDragFlag(true);

				list = new Component();
				list->addMouseListener(this, false);
				setViewedComponent(list);

				dropCursorComponent = new DropCursorComponent();
				dropCursorComponent->setAlwaysOnTop(true);
				list->addChildComponent(dropCursorComponent);

				if (direction == Direction::Horizontal)
				{
					flexBox.flexDirection = FlexBox::Direction::row;
				}
				else
				{
					flexBox.flexDirection = FlexBox::Direction::column;
				}	

				setScrollBarsShown(false, false, false, false);
			}

				
			virtual ~FlexList()
			{

			}

			void resized()
			{
				const Point<int> viewPosition = getViewPosition();
				const Rectangle<int> bounds = getLocalBounds();

				if (direction == Direction::Horizontal)
				{
					const int itemHeight = bounds.getHeight();					
					int listWidth = 0;

					for (FlexItem& flexItem : flexBox.items)
					{
						listWidth += (int)flexItem.width;
						flexItem.height = (float)itemHeight;
						flexItem.associatedComponent->setBounds(0, 0, (int)flexItem.width, itemHeight);
					}

					list->setBounds(0, 0, jmax(listWidth, getWidth()), itemHeight);
					flexBox.performLayout(Rectangle<int>(0, 0, listWidth, itemHeight));
				}
				else
				{
					const int itemWidth = bounds.getWidth();
					int listHeight = 0;

					for (FlexItem& flexItem : flexBox.items)
					{
						listHeight += (int)flexItem.height;
						flexItem.width = (float)itemWidth;
						flexItem.associatedComponent->setBounds(0, 0, itemWidth, (int)flexItem.height);
					}

					list->setBounds(0, 0, itemWidth, jmax(listHeight, getHeight()));
					flexBox.performLayout(Rectangle<int>(0, 0, itemWidth, listHeight));
				}

				Viewport::resized();
				setViewPosition(viewPosition);
			}

			void refresh()
			{
				resized();
			}

			void refresh(Point<int> viewPosition)
			{
				resized();
				setViewPosition(viewPosition);
			}

			void scrollToEnd()
			{
				setViewPositionProportionately(1, 1);
			}

			void addItem(ItemType* newItem, int dimension, bool refreshView = true, bool retainPosition = true)
			{
				addItem(newItem, dimension, -1, refreshView, retainPosition);
			}

			void addItem(ItemType* newItem, int dimension, int position, bool refreshView = true, bool retainPosition = true)
			{
				items.insert(position, newItem);

				FlexItem flexItem;
				if (direction == Direction::Horizontal)
					flexItem = FlexItem(*newItem).withWidth((float)dimension).withHeight((float)newItem->getHeight());
				else
					flexItem = FlexItem(*newItem).withWidth((float)newItem->getWidth()).withHeight((float)dimension);

				flexBox.items.insert(position, flexItem);
				list->addAndMakeVisible(newItem);

				if (refreshView)
				{
					refresh();

					if (!retainPosition)
						scrollToEnd();
				}
			}

			void removeItem(ItemType* itemToDelete)
			{
				for (FlexItem& item : flexBox.items)
				{
					if (item.associatedComponent == itemToDelete)
					{
						flexBox.items.remove(&item);
						items.removeObject(itemToDelete);
						refresh();
						return;
					}
				}
			}

			void removeAllItems()
			{
				flexBox.items.clearQuick();
				items.clearQuick(true);
				refresh();
			}

			void moveItem(int sourceIndex, int targetIndex)
			{
				flexBox.items.move(sourceIndex, targetIndex);
				items.move(sourceIndex, targetIndex);
				refresh();
			}

			const OwnedArray<ItemType>& getItems()
			{
				return items;
			}

			int getNumItems() const
			{
				return items.size();
			}

			virtual bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
			{
				if (!swapGestureEnabled)
					return false;

				if (ItemType* item = dynamic_cast<ItemType*>(dragSourceDetails.sourceComponent.get()))
				{
					return items.contains(item);
				}

				return false;
			}

			virtual void itemDragEnter(const SourceDetails&) override
			{
				if (!swapGestureEnabled)
					return;

				dropCursorComponent->setVisible(true);
				beginDragAutoRepeat(10);
			}

			virtual void itemDragExit(const SourceDetails&) override
			{
				if (!swapGestureEnabled)
					return;

				dropCursorComponent->setVisible(false);
			}

			virtual void itemDragMove(const SourceDetails& dragSourceDetails) override
			{
				if (!swapGestureEnabled)
					return;

				updateDropCursorPosition(dragSourceDetails);
				autoScroll(dragSourceDetails.localPosition.x, dragSourceDetails.localPosition.y, autoScrollDistance, autoScrollSpeed);
			}

			void setAutoScrollSpeed(int newSpeed)
			{
				autoScrollSpeed = newSpeed;
			}

			void setAutoScrollDistance(int newDistance)
			{
				autoScrollDistance = newDistance;
			}

			virtual void itemDropped(const SourceDetails& dragSourceDetails) override
			{
				if (!swapGestureEnabled)
					return;

				dropCursorComponent->setVisible(false);
				beginDragAutoRepeat(-1);

				int targetIndex = flexBox.items.size();
				int distanceFromStart = 0;

				if (direction == Direction::Horizontal)
				{
					const int dropX = dragSourceDetails.localPosition.x + getViewPositionX();

					int itemIndex = 0;
					for (FlexItem& flexItem : flexBox.items)
					{
						if (dropX < distanceFromStart + (int)(flexItem.width * 0.5f))
						{
							targetIndex = itemIndex;
							break;
						}

						distanceFromStart += (int)flexItem.width;
						++itemIndex;
					}
				}
				else
				{
					const int dropY = dragSourceDetails.localPosition.y + getViewPositionY();

					int itemIndex = 0;
					for (FlexItem& flexItem : flexBox.items)
					{
						if (dropY < distanceFromStart + (int)(flexItem.height * 0.5f))
						{
							targetIndex = itemIndex;
							break;
						}

						distanceFromStart += (int)flexItem.height;
						++itemIndex;
					}
				}

				const int sourceIndex = getItemIndex(dragSourceDetails.sourceComponent);

				if (sourceIndex < targetIndex)
					targetIndex--;

				jassert(sourceIndex >= 0);
				jassert(targetIndex >= 0 && targetIndex <= flexBox.items.size());

				listeners.call(&Listener::swapGestureCallback, this, sourceIndex, targetIndex);
			}


			void updateDropCursorPosition(const SourceDetails& dragSourceDetails)
			{
				const int cursorSize = 6;
				const int cursorSizeHalf = (int)(cursorSize * 0.5f);
				
				if (direction == Direction::Horizontal)
				{
					float distanceFromStart = 0;
					const float dropX = (float)dragSourceDetails.localPosition.x + getViewPositionX();
					
					for (FlexItem& flexItem : flexBox.items)
					{
						if (dropX < distanceFromStart + flexItem.width * 0.5f)
							break;

						distanceFromStart += flexItem.width;
					}

					if (distanceFromStart <= 0)
						dropCursorComponent->setBounds(0, 0, cursorSizeHalf, getHeight());
					else
						dropCursorComponent->setBounds((int)distanceFromStart - cursorSizeHalf, 0, cursorSize, getHeight());
				}
				else
				{
					float distanceFromStart = 0;
					const float dropY = (float)dragSourceDetails.localPosition.y + getViewPositionY();

					for (const FlexItem& flexItem : flexBox.items)
					{
						if (dropY < distanceFromStart + flexItem.height * 0.5f)
							break;

						distanceFromStart += flexItem.height;
					}

					if (distanceFromStart <= 0)
						dropCursorComponent->setBounds(0, 0, getWidth(), cursorSizeHalf);
					else
						dropCursorComponent->setBounds(0, (int)distanceFromStart - cursorSizeHalf, getWidth(), cursorSize);
				}
			}

			int getItemIndex(const Component* component) const
			{
				int itemIndex = 0;
				for (const FlexItem& flexItem : flexBox.items)
				{
					if (flexItem.associatedComponent == component)
					{
						return itemIndex;
					}

					++itemIndex;
				}
				
				jassertfalse;
				return -1;
			}

			int getIdealSize() const
			{
				float dimension = 0;

				if (direction == Direction::Horizontal)
				{
					for (const FlexItem& flexItem : flexBox.items)
					{
						dimension += flexItem.width;
					}

					return (int)jmin(dimension, getParentMonitorArea().getWidth() * 0.9f);
				}
				else
				{
					for (const FlexItem& flexItem : flexBox.items)
					{
						dimension += flexItem.height;
					}

					return (int)jmin(dimension, getParentMonitorArea().getHeight() * 0.9f);
				}
			}

			virtual void mouseUp(const MouseEvent& event) override
			{
				if (event.eventComponent == list && event.mods.isLeftButtonDown() && !event.source.hasMouseMovedSignificantlySincePressed())
					listeners.call(&Listener::clickedToEmptySpace, this, event);
			}

			void addListener(Listener* listener) { listeners.add(listener); }
			void removeListener(Listener* listener) { listeners.remove(listener); }

			void getSwapGestureEnabled() const { return swapGestureEnabled; }
			void setSwapGetsureEnabled(bool enabled) { swapGestureEnabled = enabled; }

		protected:
			OwnedArray<ItemType> items;
			WeakReference<Component> list;
			FlexBox flexBox;
			Direction direction;

			int autoScrollDistance = 100;
			int autoScrollSpeed = 15;
			bool swapGestureEnabled = false;
			ScopedPointer<DropCursorComponent> dropCursorComponent;
			ListenerList<Listener> listeners;

			class EmptyPlaceholderComponent : public Component
			{
			public:
				void paint(Graphics& g) 
				{ 
					g.setColour(Colour(200, 200, 200));
					g.drawFittedText("Empty", getLocalBounds(), Justification::centred, 1); 
				}
			};

			ScopedPointer<EmptyPlaceholderComponent> emptyPlaceholderComponent;

			void addEmptyPlaceholder(int width, int height, bool refreshView = true)
			{
				if (emptyPlaceholderComponent != nullptr)
					return;

				emptyPlaceholderComponent = new EmptyPlaceholderComponent();
				emptyPlaceholderComponent->setSize(width, height);

				FlexItem flexItem;
				if (direction == Direction::Horizontal)
					flexItem = FlexItem(*emptyPlaceholderComponent).withWidth((float)width).withHeight((float)height);
				else
					flexItem = FlexItem(*emptyPlaceholderComponent).withWidth((float)width).withHeight((float)height);

				flexBox.items.add(flexItem);
				list->addAndMakeVisible(emptyPlaceholderComponent);

				if (refreshView)
					refresh();
			}

			void removeEmptyPlaceholder(bool refreshView = true)
			{
				if (emptyPlaceholderComponent == nullptr)
					return;

				for (FlexItem& item : flexBox.items)
				{
					if (item.associatedComponent == emptyPlaceholderComponent)
					{
						flexBox.items.remove(&item);
						emptyPlaceholderComponent = nullptr;

						if (refreshView)
							refresh();

						return;
					}
				}
			}

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlexList)
		};
	}
}