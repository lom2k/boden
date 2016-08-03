#ifndef BDN_Window_H_
#define BDN_Window_H_

#include <bdn/View.h>

#include <bdn/IWindowCore.h>
#include <bdn/IUiProvider.h>

namespace bdn
{

/** A top level window.	

	Windows have a single child view (the "content view") that displays the window
	contents. Usually this content view will be a view container object,
	which can then contain multiple child views.

	Windows are initially invisible. Once you have finished initializing your window
	you need to explicitly show it by setting the property visible()=true.

	Example:

	\code

	P<Window> pWindow = newObj<Window>();

	pWindow->title() = "My Window Title";

	pWindow->visible() = true;

	// the window is now visible on the screen.

	\endcode
*/
class Window : public View
{
public:
	/** @param pUiProvider the UI provider that the window should use.
			See the IUiProvider documentation for more information.
			If this is nullptr then the default UI provider for the platform is used.*/
	Window(IUiProvider* pUiProvider = nullptr);

    ~Window();


	/** Sets the specified view as the content view of the window.
		Note that windows can only have a single child content view. If one is already
		set then it will be replaced.
		See #Window class documentation for more information.*/
	void setContentView(View* pContentView)
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		if(pContentView!=_pContentView)
		{
			if(_pContentView!=nullptr)
				_pContentView->_setParentView(nullptr);

			_pContentView = pContentView;

            if(_pContentView!=nullptr)
			    _pContentView->_setParentView(this);
		}
	}


	/** Returns the window's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	P<View> getContentView()
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pContentView;
	}


	/** Returns the window's content view (see #getContentView()).
		This can be nullptr if no content view has been set yet.*/
	P<const View> getContentView() const
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		return _pContentView;
	}



	/** Tells the window to auto-size itself. The window size will be adapted according
		to the preferred size of the content view. The window also takes other considerations
		into account, like the size of the screen it is on.
		
		Note that the update happens asynchronously, so the window will usually not have the
		new size yet when the function returns.
		*/
	void requestAutoSize();


	/** Tells the window to center itself on the screen.

		The update will happen asynchronously, so the position might not have been updated yet
		when the function returns.

		If another asynchronous operation has been scheduled (like requestAutoSize())
		then it is guaranteed that the centering happens after that operation is complete.
	*/
	void requestCenter();


	/** Returns the window's title property.

		Depending on the platform, the title may or may not be visible in a window title bar on
		the screen.
		On some platforms a window does not have a title bar, so the title may be invisible.
		However, it can also be used in other places (e.g. when switching between windows, etc.),
		so it should always be set to a reasonable string.

		It is safe to use the property from any thread.
		*/
	Property<String>& title()
	{
		return _title;
	}

	const ReadProperty<String>& title() const
	{
		return _title;
	}

	

	/** Static function that returns the type name for #Window objects.*/
	static String getWindowCoreTypeName()
	{
		return "bdn.WindowCore";
	}

	String getCoreTypeName() const override
	{
		return getWindowCoreTypeName();
	}


	void getChildViews(std::list< P<View> >& childViews) const override
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		if(_pContentView!=nullptr)
			childViews.push_back(_pContentView);	
	}
		
	P<View> findPreviousChildView(View* pChildView) override
	{
		// we do not have multiple child views with an order - just a single content view
		return nullptr;
	}


	void _childViewStolen(View* pChildView) override
	{
		MutexLock lock( getHierarchyAndCoreMutex() );

		if(pChildView==_pContentView)
			_pContentView = nullptr;
	}


	Size	calcPreferredSize() const override;
	int		calcPreferredHeightForWidth(int width) const override;
	int		calcPreferredWidthForHeight(int height) const override;

	

protected:
	void layout() override;	

	void autoSize();
	void center();
	
	// allow the coordinator to call our protected functions like autoSize.
	friend class LayoutCoordinator;


	P<IUiProvider> determineUiProvider() override
	{
		// our Ui provider never changes. Just return the current one.
		return _pUiProvider;
	}

	DefaultProperty<String> _title;

private:
	P<View>					_pContentView;
};

}

#endif

