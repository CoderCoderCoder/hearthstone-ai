#pragma once

namespace FlowControl
{
	namespace Context
	{
		struct AuraGetTargets;
		struct AuraApplyOn;
		struct AuraRemoveFrom;
	}
}

namespace state {
	namespace utils { class TargetsGenerator; }

	namespace Cards {
		namespace aura {
			class AuraHandler
			{
			public:
				typedef void FuncGetTargets(FlowControl::Context::AuraGetTargets context);
				typedef void FuncApplyOn(FlowControl::Context::AuraApplyOn context);
				typedef void FuncRemoveFrom(FlowControl::Context::AuraRemoveFrom context);

				AuraHandler() :
					get_targets(nullptr), apply_on(nullptr),
					remove_from(nullptr), get_targetor_helper(nullptr)
				{
				}

				FuncGetTargets * get_targets;
				FuncApplyOn * apply_on;
				FuncRemoveFrom * remove_from;

			public: // aux fields for aura handler
				// Note: These fields are added into aura handler to prevent the use of std::function
				//       Thus reduce the 'get_targets' to be a pure function pointer
				typedef void FuncGetTargetorInfoGetter(FlowControl::Context::AuraGetTargets & context, utils::TargetsGenerator & targetor);
				FuncGetTargetorInfoGetter *get_targetor_helper;
			};
		}
	}
}