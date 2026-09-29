#pragma once
#include <string>
#include <vector>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::XML
{
    struct STSTMLIB_XML
    {
        struct TEMPLATELIB
        {
            struct LXDTEMPLATE
            {
                struct TEMPLATE
                {
                    std::string TemplateID;
                    std::string Description;
                };

                using TEMPLATELIST = std::vector<TEMPLATE>;

                std::string TemplateType;
                std::string NumberOfTemplate;
                TEMPLATELIST TemplateList;
            };

            using LCDTEMPLATE = LXDTEMPLATE;
            using LEDTEMPLATE = LXDTEMPLATE;

            std::string DatabaseLength;
            std::string Version;
            std::string UploadedBy;
            std::string UploadDate;
            LCDTEMPLATE lcdemgtemplate;
            LEDTEMPLATE ledemgtemplate;
            LCDTEMPLATE lcdtemplate;
            LEDTEMPLATE ledtemplate;
        };

        TEMPLATELIB TemplateLib;
    };

    using STSTMLIB_XML_PTR = std::shared_ptr<STSTMLIB_XML>;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace XML;
}
