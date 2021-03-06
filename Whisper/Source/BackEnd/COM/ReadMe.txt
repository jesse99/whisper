This package contains code to implement a COM-like object model. This package makes it very
easy to create Photoshop style plugins. It also makes it possible to create very modular apps
where major pieces of the app are broken out into plugins. Note that there are some notable
differences from Microsoft's COM:

1) Interfaces are aggregated into "bosses". When you query for an interface the object
model asks the boss if it contains that interface. The object model learns about bosses
via an XML file. For example:

	<Boss name = "Plugins Boss" singleton = "true">
		<Interface name = "IPlugins" impl = "CPlugins"/>
	</Boss>
	
This tells the object model that there's a boss called "Plugins Boss" and it has one interface
named "IPlugins" that is implemented with an implementation class called "CPlugins". In addition
this is a singleton boss which means that it is created automatically when the plugin it resides
in is loaded and destroyed when the plugin is unloaded.

The interface and implementation names can be pretty much anything. You inform the object model
of the mapping between the name and the class via some code executed during startup:

	REGISTER_INTERFACE_NAME(IPlugins, "IPlugins");		// "IPlugins" maps to the type IPlugins
	REGISTER_INTERFACE_FACTORY(CPlugins, L"CPlugins");	// "CPlugins" maps to the type CPlugins

In the file that defines your implementation class you need to create a factory function for the
object model. You can do that like this:
	DEFINE_INTERFACE_FACTORY(CPlugins)


2) The only way to acquire an interface is through XInterfacePtr<T>. There are three ctors that are
commonly used. One does a query and one creates a new boss (if the boss isn't a singleton boss):

	XInterfacePtr(XUnknown* unknown);
	
	template<class U>
	XInterfacePtr(const XInterfacePtr<U>& unknown);
	
	explicit XInterfacePtr(const std::wstring& boss);

If the query fails mPtr will be nil. Because interfaces are bottlenecked through XInterfacePtr all
reference counting is automatic.


3) Exceptions are used to report errors. Note that this means that you can only compile plugins
with compilers that have compatible exception models.


4) Multiple instances of an interface can be added to one boss (at startup using the XML file or
dynamicly at runtime). To avoid surprises XInterfacePtr will assert if you attempt to use the 
normal ctors to query for an interface that has multiple instances. What you need to do instead
is use the XInterfacePtr ctor that takes an XBoss::iterator. However this is something that clients 
will rarely do. More often you'll use something like draw extensions that hide all the messy details 
and allow you to dynamicly or staticly add objects that do things like frame their owner.
