// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "HashCommand.h"
#include "Workflows/WorkflowBase.h"
#include "Resources.h"

namespace AppInstaller::CLI
{
    using namespace std::string_view_literals;

    std::vector<Argument> HashCommand::GetArguments() const
    {
        return {
            Argument::ForType(Execution::Args::Type::HashFile),
            Argument::ForType(Execution::Args::Type::Msix),
        };
    }

    std::string HashCommand::ShortDescription() const
    {
        return Resources::GetInstance().ResolveWingetString(L"HashHelperDescription");
    }

    std::string HashCommand::GetLongDescription() const
    {
        return Resources::GetInstance().ResolveWingetString(L"HashHelperDescription");
    }

    std::string HashCommand::HelpLink() const
    {
        return "https://aka.ms/winget-command-hash";
    }

    void HashCommand::ExecuteInternal(Execution::Context& context) const
    {
        context <<
            Workflow::VerifyFile(Execution::Args::Type::HashFile) <<
            [](Execution::Context& context)
        {
            auto inputFile = context.Args.GetArg(Execution::Args::Type::HashFile);
            std::ifstream inStream{ inputFile, std::ifstream::binary };

            context.Reporter.Info() << "File Hash: " + Utility::SHA256::ConvertToString(Utility::SHA256::ComputeHash(inStream)) << std::endl;

            if (context.Args.Contains(Execution::Args::Type::Msix))
            {
                try
                {
                    Msix::MsixInfo msixInfo{ inputFile };
                    auto signature = msixInfo.GetSignature();
                    auto signatureHash = Utility::SHA256::ComputeHash(signature.data(), static_cast<uint32_t>(signature.size()));

                    context.Reporter.Info() << "Signature Hash: " + Utility::SHA256::ConvertToString(signatureHash) << std::endl;
                }
                catch (const wil::ResultException& re)
                {
                    context.Reporter.Warn() << 
                        "Failed to calculate MSIX signature hash." << std::endl <<
                        "Please verify that the input file is a valid, signed MSIX." << std::endl;
                    AICLI_TERMINATE_CONTEXT(re.GetErrorCode());
                }
            }
        };
    }
}
