#ifndef CONSTANTS_H
#define CONSTANTS_H

const char *const ALTERATOR_MANAGER_SERVICE_NAME            = "ru.basealt.alterator";
const char *const ALTERATOR_MANAGER_PATH                    = "/ru/basealt/alterator";
const char *const ALTERATOR_MANAGER_INTERFACE_NAME          = "ru.basealt.alterator.manager";
const char *const ALTERATOR_MANAGER_GET_OBJECTS_METHOD_NAME = "GetObjects";
const char *const ALTERATOR_MANAGER_SET_ENV_METHOD_NAME     = "SetEnvValue";

const char *const GLOBAL_PATH                           = "/ru/basealt/alterator/global";
const char *const COMPONENT_CATEGORIES_INTERFACE_NAME   = "ru.basealt.alterator.component_categories1";
const char *const COMPONENT_CATEGORIES_LIST_METHOD_NAME = "List";
const char *const COMPONENT_CATEGORIES_INFO_METHOD_NAME = "Info";

const char *const APT1_INTERFACE_NAME      = "ru.basealt.alterator.apt1";
const char *const APT1_PATH                = "/ru/basealt/alterator/apt";
const char *const APT1_INSTALL_METHOD_NAME = "Install";
const char *const APT1_REMOVE_METHOD_NAME  = "Remove";

const char *const RPM1_PATH                       = "/ru/basealt/alterator/rpm";
const char *const RPM1_INTERFACE_NAME             = "ru.basealt.alterator.rpm1";
const char *const RPM1_INTERFACE_LIST_METHOD_NAME = "List";

const char *const COMPONENT1_INTERFACE_NAME         = "ru.basealt.alterator.component1";
const char *const COMPONENT_INFO_METHOD_NAME        = "Info";
const char *const COMPONENT_DESCRIPTION_METHOD_NAME = "Description";
const char *const COMPONENT_STATUS_METHOD_NAME      = "Status";

const char *const ALTERATOR_SECTION_NAME = "Alterator Entry";

const char *const COMPONENT_NAME_KEY_NAME         = "Name";
const char *const COMPONENT_OBJECT_TYPE_KEY_NAME  = "Type";
const char *const COMPONENT_TYPE_KEY_VALUE        = "Component";
const char *const COMPONENT_DISPLAY_NAME_KEY_NAME = "DisplayName";
const char *const COMPONENT_TYPE_KEY_NAME         = "Component";
const char *const COMPONENT_COMMENT_KEY_NAME      = "Comment";
const char *const COMPONENT_CATEGORY_KEY_NAME     = "Category";
const char *const COMPONENT_PACKAGES_KEY_NAME     = "Packages";

const char *const DEFAULT_CATEGORY_ID           = "__default";
const char *const DEFAULT_CATEGORY_DISPLAY_NAME = "Other";

#endif // CONSTANTS_H
