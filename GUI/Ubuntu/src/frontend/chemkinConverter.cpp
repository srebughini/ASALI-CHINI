/*##############################################################################################
#                                                                                              #
#     #############       #############       #############       ####                ####     #
#    #             #     #             #     #             #     #    #              #    #    #
#    #    #####    #     #    #########      #    #####    #     #    #              #    #    #
#    #    #   #    #     #    #              #    #   #    #     #    #              #    #    #
#    #    #####    #     #    #              #    #####    #     #    #              #    #    #
#    #             #     #    #########      #             #     #    #              #    #    #
#    #             #     #             #     #             #     #    #              #    #    #
#    #    #####    #      #########    #     #    #####    #     #    #              #    #    #
#    #    #   #    #              #    #     #    #   #    #     #    #              #    #    #
#    #    #   #    #      #########    #     #    #   #    #     #    #########      #    #    #
#    #    #   #    #     #             #     #    #   #    #     #             #     #    #    #
#     ####     ####       #############       ####     ####       #############       ####     #
#                                                                                              #
#   Author: Stefano Rebughini <ste.rebu@outlook.it>                                            #
#                                                                                              #
################################################################################################
#                                                                                              #
#   License                                                                                    #
#                                                                                              #
#   This file is part of ASALI.                                                                #
#                                                                                              #
#   ASALI is free software: you can redistribute it and/or modify                              #
#   it under the terms of the GNU General Public License as published by                       #
#   the Free Software Foundation, either version 3 of the License, or                          #
#   (at your option) any later version.                                                        #
#                                                                                              #
#   ASALI is distributed in the hope that it will be useful,                                   #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of                             #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                              #
#   GNU General Public License for more details.                                               #
#                                                                                              #
#   You should have received a copy of the GNU General Public License                          #
#   along with ASALI. If not, see <http://www.gnu.org/licenses/>.                              #
#                                                                                              #
##############################################################################################*/

#include "frontend/chemkinConverter.hpp"

namespace ASALI
{
    chemkinConverter::chemkinConverter()
        : buttonBox_(Gtk::ORIENTATION_VERTICAL),
          output_("Convert"),
          help_("Help"),
          exit_("Exit"),
          logo_(this->relative_path_to_absolute_path("images/SmallLogo.png"))
    {
        #include "shared/Beer.H"
        #include "shared/BeerShort.H"

        files_.resize(5);

        this->set_default_size(600, 400);
        this->set_border_width(15);
        this->set_title("ASALI: CHEMKIN converter");
        this->set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        this->set_icon_from_file(this->relative_path_to_absolute_path("images/Icon.png"));

        //Adding logo
        mainBox_.set_halign(Gtk::ALIGN_START);
        mainBox_.set_spacing(10);
        mainBox_.pack_start(logo_, Gtk::PACK_SHRINK);

        //Adding thermo and transport buttons
        mainBox_.pack_start(buttonBox_, Gtk::PACK_SHRINK);
        buttonBox_.set_valign(Gtk::ALIGN_CENTER);
        buttonBox_.set_spacing(10);
        buttonBox_.set_homogeneous(true);
        buttonBox_.pack_start(kinetic_, Gtk::PACK_SHRINK);
        kinetic_.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this, &chemkinConverter::load), 0));
        buttonBox_.pack_start(thermo_, Gtk::PACK_SHRINK);
        thermo_.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this, &chemkinConverter::load), 1));
        buttonBox_.pack_start(transport_, Gtk::PACK_SHRINK);
        transport_.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this, &chemkinConverter::load), 2));
        buttonBox_.pack_start(surface_, Gtk::PACK_SHRINK);
        surface_.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(*this, &chemkinConverter::load), 3));
        buttonBox_.pack_start(endGrid_, Gtk::PACK_SHRINK);

        endGrid_.set_column_homogeneous(true);
        endGrid_.set_column_spacing(10);
        endGrid_.set_row_spacing(10);
        endGrid_.attach(output_, 0, 0, 1, 1);
        endGrid_.attach(help_, 1, 0, 1, 1);
        endGrid_.attach(exit_, 2, 0, 1, 1);

        output_.signal_clicked().connect(sigc::mem_fun(*this, &chemkinConverter::run));
        help_.signal_clicked().connect(sigc::mem_fun(*this, &chemkinConverter::help));
        exit_.signal_clicked().connect(sigc::mem_fun(*this, &chemkinConverter::exit));

        this->clean();
    }

    void chemkinConverter::clean()
    {
        this->remove();
        this->add(mainBox_);
        kinetic_.set_label("Gas phase chemistry file");
        thermo_.set_label("Thermodynamic coefficient file");
        transport_.set_label("Transport coefficient file");
        surface_.set_label("Surface chemistry file");

        for (unsigned int i = 0; i < 5; i++)
        {
            files_[i] = "";
        }

        this->resize(mainBox_.get_width(), mainBox_.get_height());
        this->show_all_children();
    }

    void chemkinConverter::exit()
    {
        this->hide();
    }

    void chemkinConverter::run()
    {
        if (files_[0] == "")
        {
            this->error(kinetic_.get_label());
        }
        else if (files_[1] == "")
        {
            this->error(thermo_.get_label());
        }
        else if (files_[2] == "")
        {
            this->error(transport_.get_label());
        }
        else if (files_[3] == "")
        {
            this->error(surface_.get_label());
        }
        else
        {
            std::string dialogname = this->save_file(this->get_toplevel()->gobj(), "*.cti");
            if (dialogname != "")
            {
                this->eraseSubString(dialogname, ".xml");
                this->eraseSubString(dialogname, ".cti");
                std::string filename = dialogname + ".cti";
                {
                    files_[4] = "--output=" + filename;
                }

                {
                    std::string cmd = "ck2cti " + files_[0] + " " + files_[1] + " " + files_[2] + " " + files_[3] + " " + files_[4];
                    system(cmd.c_str());
                }

                if (bool(std::ifstream(filename)))
                {
                    std::string cmd = "ctml_writer " + filename + " " + dialogname + ".xml";
                    system(cmd.c_str());
                    std::remove(filename.c_str());
                    this->savedMessage();
                }
                else
                {
                    this->notSavedMessage();
                }
            }
        }
    }

    void chemkinConverter::load(int index)
    {
        std::string filename = this->open_file(this->get_toplevel()->gobj());
        if (filename != "")
        {
            std::string command;
            if (index == 0)
            {
                kinetic_.set_label(filename);
                command = "--input=" + filename;
            }
            else if (index == 1)
            {
                thermo_.set_label(filename);
                command = "--thermo=" + filename;
            }
            else if (index == 2)
            {
                transport_.set_label(filename);
                command = "--transport=" + filename;
            }
            else if (index == 3)
            {
                surface_.set_label(filename);
                command = "--surface=" + filename;
            }

            files_[index] = command;
            this->resize(mainBox_.get_width(), mainBox_.get_height());
            this->show_all_children();
        }
    }

    void chemkinConverter::savedMessage()
    {
        Gtk::MessageDialog dialog(*this, "Conversion completed. Your file as been saved.\nThank you for using ASALI.", true, Gtk::MESSAGE_OTHER);
        dialog.set_secondary_text(this->getBeerShort(), true);
        dialog.run();
    }

    void chemkinConverter::help()
    {
        Gtk::MessageDialog dialog(*this, "Problem in converting your scheme into CANTERA format?\nSend it to: ste.rebu@outlook.it\nWe will be glad to help you!", true, Gtk::MESSAGE_OTHER);
        dialog.set_secondary_text(this->getBeerShort(), true);
        dialog.run();
    }

    void chemkinConverter::error(std::string type)
    {
        Gtk::MessageDialog dialog(*this, type + " is missing!!", true, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(this->getBeerShort(), true);
        dialog.run();
    }

    void chemkinConverter::notSavedMessage()
    {
        Gtk::MessageDialog dialog(*this, "Conversion stopped! The most common reasons for that are:\n1/Something is wrong with your input files.\n2/You have not load CANTERA enviornment", true, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(this->getBeerShort(), true);
        dialog.run();
    }

    bool chemkinConverter::checkConvertedFile(std::string filename)
    {
        std::ifstream input;
        const char *path = filename.c_str();
        input.open(path);
        std::vector<std::string> readed(2);
        readed[0] = "none";
        readed[1] = "none";
        {
            std::vector<std::string> a;
            std::vector<std::string> b;
            while (!input.eof())
            {
                std::string line;
                getline(input, line);
                if (line.find("<phase ") != std::string::npos)
                {
                    a.push_back(line);
                }
                else if (line.find("<kinetics ") != std::string::npos)
                {
                    b.push_back(line);
                }
            }

            for (unsigned int i = 0; i < b.size(); i++)
            {
                if (b[i].find("Interface") != std::string::npos)
                {
                    readed[1] = a[i];
                }
                else if (b[i].find("GasKinetics") != std::string::npos)
                {
                    readed[0] = a[i];
                }
            }
        }

        if (readed[0] == "none" ||
            readed[1] == "none")
        {
            std::string type = "none";
            for (unsigned int i = 0; i < readed.size(); i++)
            {
                if (readed[i] != "none")
                {
                    std::string dummyString = readed[i];

                    for (std::size_t j = 0; j < dummyString.size(); j++)
                    {
                        if (dummyString.substr(j, 1) == ">")
                        {
                            dummyString.replace(j, 1, " ");
                        }
                        else if (dummyString.substr(j, 1) == "\"")
                        {
                            dummyString.replace(j, 1, " ");
                        }
                        else if (dummyString.substr(j, 1) == "=")
                        {
                            dummyString.replace(j, 1, " ");
                        }
                    }

                    std::vector<std::string> dummyVector;
                    dummyVector.clear();

                    std::istringstream iss(dummyString);
                    while (iss >> dummyString)
                    {
                        dummyVector.push_back(dummyString);
                    }

                    if (dummyVector.size() > 4)
                    {
                        for (unsigned int j = 0; j < dummyVector.size() - 1; j++)
                        {
                            if (dummyVector[j] == "<phase" &&
                                dummyVector[j + 1] == "dim" &&
                                dummyVector[j + 3] == "id")
                            {
                                type = dummyVector[j + 4];
                                type.erase(std::remove(type.begin(), type.end(), '"'), type.end());
                                type.erase(std::remove(type.begin(), type.end(), ' '), type.end());
                            }
                            else if (dummyVector[j] == "<phase" &&
                                     dummyVector[j + 1] == "id" &&
                                     dummyVector[j + 3] == "dim")
                            {
                                type = dummyVector[j + 2];
                                type.erase(std::remove(type.begin(), type.end(), '"'), type.end());
                                type.erase(std::remove(type.begin(), type.end(), ' '), type.end());
                            }
                        }
                    }
                }
            }
            if (type == "none")
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            std::vector<std::string> type(2);
            type[0] = "none";
            type[1] = "none";
            for (unsigned int i = 0; i < readed.size(); i++)
            {
                std::string dummyString = readed[i];

                for (std::size_t j = 0; j < dummyString.size(); j++)
                {
                    if (dummyString.substr(j, 1) == ">")
                    {
                        dummyString.replace(j, 1, " ");
                    }
                    else if (dummyString.substr(j, 1) == "\"")
                    {
                        dummyString.replace(j, 1, " ");
                    }
                    else if (dummyString.substr(j, 1) == "=")
                    {
                        dummyString.replace(j, 1, " ");
                    }
                }

                std::vector<std::string> dummyVector;
                dummyVector.clear();

                std::istringstream iss(dummyString);
                while (iss >> dummyString)
                {
                    dummyVector.push_back(dummyString);
                }

                if (dummyVector.size() > 4)
                {
                    for (unsigned int j = 0; j < dummyVector.size() - 1; j++)
                    {
                        if (dummyVector[j] == "<phase" &&
                            dummyVector[j + 1] == "dim" &&
                            dummyVector[j + 3] == "id")
                        {
                            type[i] = dummyVector[j + 4];
                            type[i].erase(std::remove(type[i].begin(), type[i].end(), '"'), type[i].end());
                            type[i].erase(std::remove(type[i].begin(), type[i].end(), ' '), type[i].end());
                        }
                        else if (dummyVector[j] == "<phase" &&
                                 dummyVector[j + 1] == "id" &&
                                 dummyVector[j + 3] == "dim")
                        {
                            type[i] = dummyVector[j + 2];
                            type[i].erase(std::remove(type[i].begin(), type[i].end(), '"'), type[i].end());
                            type[i].erase(std::remove(type[i].begin(), type[i].end(), ' '), type[i].end());
                        }
                    }
                }

                if (type[0] != "none" && type[1] != "none")
                {
                    break;
                }
            }

            if (type[0] == "none" ||
                type[1] == "none")
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }

    void chemkinConverter::eraseSubString(std::string &mainStr, const std::string toErase)
    {
        size_t pos = std::string::npos;

        while ((pos = mainStr.find(toErase)) != std::string::npos)
        {
            mainStr.erase(pos, toErase.length());
        }
    }

    std::string chemkinConverter::getBeer()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<const unsigned int> distribution(0, beer_.size()-1);
        int i = distribution(gen);
        return beer_[i];
    }

    std::string chemkinConverter::getBeerShort()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<const unsigned int> distribution(0, beerShort_.size()-1);
        int i = distribution(gen);
        return beerShort_[i];
    }

    chemkinConverter::~chemkinConverter()
    {
    }
}
