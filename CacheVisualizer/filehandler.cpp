#include "filehandler.h"
/*Private checkers*/
bool FileHandler::checkAttValValidityInt(const std::string &inValue, int l_limit)
{
    int value;
    try {
        value = std::stoi(inValue);
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&){
        return false;
    }
    if(value < l_limit){
        return false;
    }
    return true;
}

bool FileHandler::checkAttValValidityIntRestrict(const std::string &inValue)
{
    int value;
    try {
        value = std::stoi(inValue);
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&){
        return false;
    }
    if(value != 0 && value != 1){
        return false;
    }
    return true;
}

bool FileHandler::checkAttValValidityLL(const std::string &inValue)
{
    long long value;
    try {
        value = std::stoll(inValue);
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&){
        return false;
    }
    if(value < 1){
        return false;
    }
    return true;
}

/*Private extraction methods (3), used by readTraceData()*/
void FileHandler::extractLaunchConfig(std::string_view o_line, ExecutionSimulation *sim) const
{
    if (o_line.substr(0,9) == "numBlocks"){
        int nb = 0;
        std::string_view block_num(o_line.substr(10, o_line.size()));
        std::from_chars(block_num.data(), block_num.data() + block_num.size(),nb);
        sim->setNumBlocks(nb);
    }

    if (o_line.substr(0,18) == "numThreadsPerBlock"){
        int tb = 0;
        std::string_view threads_pb(o_line.substr(19, o_line.size()));
        std::from_chars(threads_pb.data(), threads_pb.data() + threads_pb.size(),tb);
        sim->setThreadsPerBlock(tb);
    }

    if (o_line.substr(0,15) == "blockDimensions"){
        int bdx = 0, bdy = 0, bdz = 0;
        std::string_view block_dim(o_line.substr(16, o_line.size()));
        /*Get X*/
        size_t first = 0;
        auto second = block_dim.find_first_of('-', first);
        std::string_view v_bx(block_dim.substr(first, second - first));
        std::from_chars(v_bx.data(), v_bx.data() + v_bx.size(), bdx);
        /*Get Y*/
        first = second + 1;
        second = block_dim.find_first_of('-', first);
        std::string_view v_by(block_dim.substr(first, second - first));
        std::from_chars(v_by.data(), v_by.data() + v_by.size(), bdy);
        /*Get Z*/
        first = second + 1;
        second = block_dim.find_first_of('-', first);
        std::string_view v_bz(block_dim.substr(first, second - first));
        std::from_chars(v_bz.data(), v_bz.data() + v_bz.size(), bdz);
        sim->setBlocksX(bdx);
        sim->setBlocksY(bdy);
        sim->setBlocksZ(bdz);

    }

    if (o_line.substr(0,16) == "threadDimensions"){
        int tx = 0, ty = 0, tz = 0;
        std::string_view thread_dim(o_line.substr(17, o_line.size()));
        size_t first = 0;
        /*Get X*/
        auto second = thread_dim.find_first_of('-', first);
        std::string_view v_tx(thread_dim.substr(first, second - first));
        std::from_chars(v_tx.data(), v_tx.data() + v_tx.size(), tx);
        first = second + 1;
        /*Get Y*/
        second = thread_dim.find_first_of('-', first);
        std::string_view v_ty(thread_dim.substr(first, second - first));
        std::from_chars(v_ty.data(), v_ty.data() + v_ty.size(), ty);
        first = second + 1;
        /*Get Z*/
        second = thread_dim.find_first_of('-', first);
        std::string_view v_tz(thread_dim.substr(first, second - first));
        std::from_chars(v_tz.data(), v_tz.data() + v_tz.size(), tz);

        sim->setThreadsX(tx);
        sim->setThreadsY(ty);
        sim->setThreadsZ(tz);
    }
}

void FileHandler::extractTraceEntry(std::string_view o_line, int &bx, int &by, int &wid, char &dname, char &oper, unsigned int &idx, unsigned long long &address, unsigned long long &time) const
{
    /*Get the string_view of the current line and keep splitting it until
      we have isolated all values.*/
    /*Block Idxs*/
    size_t first = 0;
    auto second = o_line.find_first_of(',',first);
    std::string_view v_b(o_line.substr(first, second-first));
    size_t f_pair = v_b.find_first_of(':',0) + 1;
    auto s_pair = v_b.find_first_of(':',f_pair);
    std::string_view v_pair(v_b.substr(f_pair, s_pair - f_pair));
    /*Triplet(0-0-0) isolated*/
    /*Get X*/
    size_t first_bidxs = 0;
    auto second_bidxs = v_pair.find_first_of('-',first_bidxs);
    std::string_view v_bidx(v_pair.substr(first_bidxs, second_bidxs - first_bidxs));
    std::from_chars(v_bidx.data(), v_bidx.data() + v_bidx.size(),bx);
    /*Get Y*/
    first_bidxs = second_bidxs + 1;
    second_bidxs = v_pair.find_first_of('-', first_bidxs);
    std::string_view v_bidy(v_pair.substr(first_bidxs, second_bidxs - first_bidxs));
    std::from_chars(v_bidy.data(), v_bidy.data() + v_bidy.size(), by);

    /*WARP ID*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_wid(o_line.substr(first, second - first));
    std::from_chars(v_wid.data(), v_wid.data() + v_wid.size(), wid);

    /*Data Structure Name*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_name(o_line.substr(first, second - first));
    dname = v_name.at(0);

    /*Operation*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_op(o_line.substr(first, second - first));
    oper = v_op.at(0);

    /*Data Structure Idx*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_idx(o_line.substr(first, second - first));
    std::from_chars(v_idx.data(), v_idx.data() + v_idx.size(), idx);

    /*ADDRESS*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_add(o_line.substr(first, second - first));
    std::from_chars(v_add.data(), v_add.data() + v_add.size(), address);

    /*TIME*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_time(o_line.substr(first, second - first));
    std::from_chars(v_time.data(), v_time.data() + v_time.size(), time);

}

void FileHandler::extractBlockScheduling(std::string_view o_line, int &bx, int &by, int &bz, int &sm, long long &gt) const
{
    /*Block Idxs*/
    size_t first = 0;
    auto second = o_line.find_first_of(',',first);
    std::string_view v_b(o_line.substr(first, second-first));
    size_t f_triplet = v_b.find_first_of(':',0) + 1;
    auto s_triplet = v_b.find_first_of(':',f_triplet);
    std::string_view v_triplet(v_b.substr(f_triplet, s_triplet - f_triplet));
    /*Triplet(0-0-0) isolated*/
    /*Get X*/
    size_t first_bidxs = 0;
    auto second_bidxs = v_triplet.find_first_of('-',first_bidxs);
    std::string_view v_bidx(v_triplet.substr(first_bidxs, second_bidxs - first_bidxs));
    std::from_chars(v_bidx.data(), v_bidx.data() + v_bidx.size(),bx);
    /*Get Y*/
    first_bidxs = second_bidxs + 1;
    second_bidxs = v_triplet.find_first_of('-', first_bidxs);
    std::string_view v_bidy(v_triplet.substr(first_bidxs, second_bidxs - first_bidxs));
    std::from_chars(v_bidy.data(), v_bidy.data() + v_bidy.size(), by);
    /*Get Z*/
    first_bidxs = second_bidxs + 1;
    second_bidxs = v_triplet.find_first_of('-', first_bidxs);
    std::string_view v_bidz(v_triplet.substr(first_bidxs, second_bidxs - first_bidxs));
    std::from_chars(v_bidz.data(), v_bidz.data() + v_bidz.size(), bz);

    /*SM*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_sm(o_line.substr(first, second-first));
    size_t first_sm = v_sm.find_first_of(':',0) + 1;
    auto second_sm = v_sm.find_first_of(':',first_sm);
    std::string_view v_sm_val(v_sm.substr(first_sm, second_sm - first_sm));
    std::from_chars(v_sm_val.data(), v_sm_val.data() + v_sm_val.size(), sm);

    /*GT*/
    first = second + 1;
    second = o_line.find_first_of(',',first);
    std::string_view v_gt(o_line.substr(first, second-first));
    size_t first_gt = v_gt.find_first_of(':',0) + 1;
    auto second_gt = v_gt.find_first_of(':',first_gt);
    std::string_view v_gt_val(v_gt.substr(first_gt, second_gt - first_gt));
    std::from_chars(v_gt_val.data(), v_gt_val.data() + v_gt_val.size(), gt);
}


FileHandler::FileHandler()
{

}

void FileHandler::initConfigMap()
{
    config_attributes["name"] = 0;
    config_attributes["cudacores"] = 0;
    config_attributes["smm"] = 0;
    config_attributes["warpsize"] = 0;
    config_attributes["concurrentwarps"] = 0;
    config_attributes["cacheline"] = 0;
    config_attributes["issectored"] = 0;
    config_attributes["sectorsize"] = 0;
    config_attributes["waysizel1"] = 0;
    config_attributes["waysizel2"] = 0;
    config_attributes["l1enabled"] = 0;
    config_attributes["replacementpolicy"] = 0;
    config_attributes["writepolicy"] = 0;
    config_attributes["global"] = 0;
    config_attributes["l2size"] = 0;
    config_attributes["l1size"] = 0;
    config_attributes["autodelay"]=0;
    config_attributes["globallatency"]=0;
}

/*Returns False if something is wrong with the configuration file, returns true otherwise.
  Only performs basic validity checks. The correctness of the configuration attributes
  should also be considered by the user (i.e. the cacheline size cannot be 1 bytes, or the
  sector size cannot be larger than the cacheline size)*/
bool FileHandler::checkConfigValidity(const std::string& fname)
{
    bool missingAttribute = false, unacceptedValue = false;

    std::ifstream config_file;
    config_file.open(fname);

    if(config_file.is_open()){
        std::string line;
        while (getline(config_file,line)){
            if(line[0] == '#' || line.empty()){continue;}
            std::istringstream tokenizer(line);
            std::string attribute, value;
            std::getline(tokenizer, attribute, '=');
            std::getline(tokenizer, value);

            qDebug("Tokenizer: att=%s,val=%s\n", attribute.c_str(), value.c_str());

            if (attribute == "name")
            {
                config_attributes["name"] = 1;
                continue;
            }
            else if (attribute == "cudacores")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["cudacores"] = 1;
                continue;
            }
            else if (attribute == "smm")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["smm"] = 1;
                continue;
            }
            else if (attribute == "warpsize")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["warpsize"] = 1;
                continue;
            }
            else if (attribute == "concurrentwarps")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["concurrentwarps"] = 1;
                continue;
            }
            else if (attribute == "cacheline")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["cacheline"] = 1;
                continue;
            }
            else if (attribute == "issectored")
            {
                if (!this->checkAttValValidityIntRestrict(value)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["issectored"] = 1;
                continue;
            }
            else if (attribute == "sectorsize")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["sectorsize"] = 1;
                continue;
            }
            else if (attribute == "waysizel1")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["waysizel1"] = 1;
                continue;
            }
            else if (attribute == "waysizel2")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["waysizel2"] = 1;
                continue;
            }
            else if (attribute == "l1enabled")
            {
                if (!this->checkAttValValidityIntRestrict(value)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["l1enabled"] = 1;
                continue;
            }
            else if (attribute == "replacementpolicy")
            {
                if (!this->checkAttValValidityIntRestrict(value)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["replacementpolicy"] = 1;
                continue;
            }
            else if (attribute == "writepolicy")
            {
                if (!this->checkAttValValidityIntRestrict(value)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["writepolicy"] = 1;
                continue;
            }
            else if (attribute == "global")
            {
                if (!this->checkAttValValidityLL(value)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["global"] = 1;
                continue;
            }
            else if (attribute == "l2size")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["l2size"] = 1;
                continue;
            }
            else if (attribute == "l1size")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["l1size"] = 1;
                continue;
            }
            else if (attribute == "globallatency")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["globallatency"] = 1;
                continue;
            }
            else if (attribute == "autodelay")
            {
                if (!this->checkAttValValidityInt(value, 1)) {
                    unacceptedValue = true;
                    break;
                }
                config_attributes["autodelay"] = 1;
                continue;
            }
            else //invalid option (ignored)
            {
                qDebug("Unknown Option\n");
            }

        }
    }

    /*If any of the attributes is missing...*/
    for (auto it = config_attributes.begin(); it != config_attributes.end(); it++){
        if (it->second != 1){
            missingAttribute = true;
            break;
        }
    }

    /*Reset the map for next read*/
    for (auto it = config_attributes.begin(); it != config_attributes.end(); it++){
        it->second = 0;
    }

    config_file.close();

    if (missingAttribute || unacceptedValue){
        return false;
    } else {
        return true;
    }
}

/*Calls checkConfigValidity to verify that the input is valid.
  Then updates the application settings with read configuration*/
bool FileHandler::readConfig(const std::string& fname)
{
    if(!this->checkConfigValidity(fname)){
        return false;
    }

    std::ifstream config_file;
    config_file.open(fname);

    if (config_file.is_open()){
        std::string line;
        while (getline(config_file,line)){
            //Ignore comments and empty lines
            if(line[0] == '#' || line.empty()){continue;}
            std::istringstream tokenizer(line);
            std::string attribute, value;
            std::getline(tokenizer, attribute, '=');
            std::getline(tokenizer, value);

            qDebug("Tokenizer(Settings): att=%s,val=%s\n", attribute.c_str(), value.c_str());

            if (attribute == "name")
            {
                Settings::setGpuName(value);
            }
            else if(attribute == "architecture")
            {
                Settings::setArchitecture(value);
            }
            else if (attribute == "chip")
            {
                Settings::setChip(value);
            }
            else if(attribute == "cudacores")
            {
                Settings::setCores(std::stoi(value));
            }
            else if(attribute == "smm")
            {
                Settings::setNumSm(std::stoi(value));
            }
            else if(attribute == "warpsize")
            {
                Settings::setWarpSize(std::stoi(value));
            }
            else if(attribute == "concurrentwarps")
            {
                Settings::setConcurrentWarps(std::stoi(value));
            }
            else if(attribute == "cacheline")
            {
                Settings::setClineSize(std::stoi(value));
            }
            else if(attribute == "issectored")
            {
                Settings::setIsSectored(std::stoi(value));
            }
            else if(attribute == "sectorsize")
            {
                Settings::setSectorSize(std::stoi(value));
            }
            else if(attribute == "waysizel1")
            {
                Settings::setWaySizeL1(std::stoi(value));
            }
            else if(attribute == "waysizel2")
            {
                Settings::setWaySizeL2(std::stoi(value));
            }
            else if(attribute == "l1enabled")
            {
                Settings::setL1Enabled(std::stoi(value));
            }
            else if(attribute == "replacementpolicy")
            {
                ReplacementPolicy val = static_cast<ReplacementPolicy>(std::stoi(value));
                Settings::setReplacePolicy(val);
            }
            else if(attribute == "writepolicy")
            {
                MemWritePolicy val = static_cast<MemWritePolicy>(std::stoi(value));
                Settings::setWritePolicy(val);
            }
            else if(attribute == "global")
            {
                Settings::setGlobalMemSize(std::stoll(value));
            }
            else if(attribute == "l2size")
            {
                Settings::setL2SizeBytes(std::stoi(value));
            }
            else if(attribute == "l1size")
            {
                Settings::setL1SizeBytes(std::stoi(value));            }
            else if(attribute == "globallatency")
            {
                Settings::setGlobalToCacheLatency(std::stoi(value));
            }
            else if(attribute == "autodelay")
            {
                Settings::setWorkerDelay(std::stoi(value));
            }
            else
            {
                qDebug("Unknown Option\n");
            }

        }
        /*Finished reading, now compute the rest of the settings*/
        Settings::computeConfig();
        config_file.close();
    } else {
        qDebug("Failed to open configuration file\n");
    }
    return true;
}


/*Method for reading in the trace data (formated according to specifications) and
  passing on the read values to the simulation objects. Implementation is based on
  Qobjects and string tokenization (relatively slow for large files). Currently this
  method is unused (only kept for reference).*/
bool FileHandler::readTraceDataFromQstream(QTextStream& trace_data, ExecutionSimulation *sim)
{
    trace_data.seek(0);
    bool seenEqual = false;
    bool seenPlus = false;
    bool allowOnce = true;


    while(!trace_data.atEnd()){
        QString line = trace_data.readLine();
        if(!line.isEmpty() && line.at(0) != '#'){
            if (line.at(0) == '='){seenEqual = true;}
            if (line.at(0) == '+'){seenPlus = true;}
            if (!seenEqual){
                QStringList linesplit = line.split(":");
                std::string att, val;
                att = linesplit[0].toStdString();
                val = linesplit[1].toStdString();
                if(att == "numBlocks"){
                    sim->setNumBlocks(std::stoi(val));
                } else if (att == "numThreadsPerBlock"){
                    sim->setThreadsPerBlock(std::stoi(val));
                } else if (att == "blockDimensions"){
                    QStringList val_split = QString::fromStdString(val).split("-");
                    if (val_split.size() != 3){
                        trace_data.flush();
                        return false;
                    } else {
                        sim->setBlocksX(val_split[0].toInt());
                        sim->setBlocksY(val_split[1].toInt());
                        sim->setBlocksZ(val_split[2].toInt());
                    }
                } else if (att == "threadDimensions"){
                    QStringList val_split = QString::fromStdString(val).split("-");
                    if (val_split.size() != 3){
                        trace_data.flush();
                        return false;
                    } else {
                        sim->setThreadsX(val_split[0].toInt());
                        sim->setThreadsY(val_split[1].toInt());
                        sim->setThreadsZ(val_split[2].toInt());
                    }
                }
            }//end launch config

            //Get the block scheduling info
            if (!seenPlus && seenEqual){
                if (line.isEmpty() || line.at(0) == '='){continue;}
                QStringList linesplit = line.trimmed().split(",");
                if (linesplit.size() != 3){
                    trace_data.flush();
                    return false;
                } else {
                    QStringList entrysplit1 = linesplit[0].split(":");
                    QStringList entrysplit2 = linesplit[1].split(":");
                    QStringList entrysplit3 = linesplit[2].split(":");
                    QStringList block_vals = entrysplit1[1].split("-");
                    if (entrysplit1.size() != 2 && entrysplit2.size() != 2 && entrysplit2.size() != 2 && block_vals.size() != 3){
                        trace_data.flush();
                        return false;
                    }
                    std::string attribute1, attribute2, attribute3;

                    attribute1 = entrysplit1[0].trimmed().toStdString();
                    attribute2 = entrysplit2[0].trimmed().toStdString();
                    attribute3 = entrysplit3[0].trimmed().toStdString();
                    block_vals = entrysplit1[1].split("-");

                    if (attribute1 == "B" && attribute2 == "SM" && attribute3 == "GT"){
                        int bx = block_vals[0].toInt();
                        int by = block_vals[1].toInt();
                        int bz = block_vals[2].toInt();
                        int smid = entrysplit2[1].toInt();
                        long long gtime = entrysplit3[1].toLongLong();
                        sim->addScheduleEntry(bx, by, smid, gtime);
                    } else {
                        trace_data.flush();
                        return false;
                    }
                }
            }//end block scheduling info

            //Finally get the trace input
            if (seenPlus && seenEqual){
                if(allowOnce){
                    sim->sortSchedulingEntries();
                    sim->configureDims();
                    sim->generateBlocks();
                    allowOnce = false;
                }
                if (line.isEmpty() || line.at(0) == '+'){continue;}
                QStringList linesplit = line.split(",");
                if (linesplit.size() != 7){
                    trace_data.flush();
                    return false;
                } else {
                    QStringList b_info = linesplit[0].split('-');
                    int blockx = b_info[0].toInt();
                    int blocky = b_info[1].toInt();
                    int wid = linesplit[1].toInt();
                    std::string dsn = linesplit[2].toStdString();
                    char dn = dsn[0];
                    char s_op = linesplit[3].toStdString()[0];
                    OperationType operation;
                    if (s_op == 'R'){
                        operation = READ;
                    } else if (s_op == 'W'){
                        operation = WRITE;
                    }
                    unsigned int ds_idx = linesplit[4].toUInt();
                    unsigned long long address = linesplit[5].toULongLong();
                    unsigned long long cycles = linesplit[6].toULongLong();
                    sim->mapAccessToBlock(blockx, blocky, wid, dn, operation, ds_idx, address, cycles);
                }
            }
        }//end if empty/comment

    }//end of trace reading
    return true;
}


/*Lightweight method for reading the memory trace files. Based on std::string_view for splitting
  the trace entries based on delimeters (instead of tokenizer), and on std::from_chars for conversion.
  (Functionality encapsulated in extractXXX() functions)
  Therefore C++ 17 and a recent compiler version is required. Due to the string_view and the conversion
  choosen, reading large files is very fast compared to the QObject method.
  Due to this visualizer being used to process very large memory traces (in the order of GB's), the reading
  and the data mapping must be very fast to support a large amount of experiments. To this end, this function
  forgoes validity and error checking so that the reading is done fast.

  The std::from_chars will fail silently (no application abrupt termination) and the variabl used to store the
  result will be unaffected. The cannonical way to check for errors is:
  std::string str = "123";
   const auto res = std::from_chars(str.data(),
                                     str.data() + str.size(),
                                     value);

    if (res.ec == std::errc())
    {
        std::cout << "value: " << value
                  << ", distance: " << res.ptr - str.data() << '\n';
    }
    else if (res.ec == std::errc::invalid_argument)
    {
        std::cout << "invalid argument!\n";
    }
    else if (res.ec == std::errc::result_out_of_range)
    {
        std::cout << "out of range! res.ptr distance: "
                  << res.ptr - str.data() << '\n';
    }

    Moreover, the std::from_chars will fail if whitespaces are introduced between delimeters, something that goes
    against the trace format specifications. (The spaces could be removed with std::remove_if). For performance reasons,
    the input trace is assumed to adhere to the memory trace specifications.
*/
bool FileHandler::readTraceData(std::ifstream& in_stream, ExecutionSimulation *sim)
{
    bool seen_equal = false, seen_plus = false, allow_once = true;


    std::string line;
    //line.reserve(100);

    while (std::getline(in_stream, line)) {
        if(!line.empty() && line.at(0) != '#' && line.at(0)!= ' '){
            /*Get a string_view for lightweight splitting*/
            std::string_view o_line(line);
            /*Flip the flags if we are past a certain point in the trace file*/
            if (line.at(0) == '='){seen_equal = true;}
            if (line.at(0) == '+'){seen_plus = true;}

            /*Process the 1st section of the trace (launch config)*/
            if(!seen_equal){

                this->extractLaunchConfig(o_line, sim);

            }//end launch config

            /*Get the block scheduling info*/
            if(!seen_plus && seen_equal){
                if(o_line.at(0) == '='){continue;}
                int bx = 0, by = 0, bz = 0, sm = 0;
                long long gt = 0;

                this->extractBlockScheduling(o_line, bx, by, bz, sm, gt);

                sim->addScheduleEntry(bx, by, sm, gt);

            }//end block scheduling info

            /*Finally get the trace input*/
            if(seen_plus && seen_equal){
                /*Use the info read so far to setup the simulation object once*/
                if(allow_once){
                    sim->sortSchedulingEntries();
                    //sim->printSchedule();
                    sim->configureDims();
                    sim->generateBlocks();
                    //sim->setStreamReserve();
                    allow_once = false;
                }
                if(o_line.at(0) == '+'){continue;}

                /*Vars for std::from_chars*/
                int bx = 0, by = 0, wid = 0;
                char dname;
                OperationType op;
                char oper;
                unsigned int idx = 0;
                unsigned long long address = 0, time = 0;

                this->extractTraceEntry(o_line, bx, by, wid, dname, oper, idx, address, time);

                if(oper == 'R'){
                    op = READ;
                } else {
                    op = WRITE;
                }

                /*Pass the extracted data to the simulation object, which will map it to the appropriate block*/
                sim->mapAccessToBlock(bx,by,wid,dname,op,idx,address,time);

            }//end trace input section
        }//end if valid line
    }//end while
    return  true;
}
